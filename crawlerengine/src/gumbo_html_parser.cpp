#include "gumbo_html_parser.h"
#include "gumbo_html_node.h"
#include "response_headers.h"
#include "parsed_page.h"
#include "page_parser_helpers.h"

namespace
{

template <class UnaryPredicate>
static std::vector<const GumboNode*> findNodesRecursive(const GumboNode* node, UnaryPredicate predicate) noexcept
{
	std::vector<const GumboNode*> result;

	if (predicate(node))
	{
		result.push_back(node);
	}

	if (node->type == GUMBO_NODE_ELEMENT)
	{
		const GumboVector* children = &node->v.element.children;

		for (unsigned int i = 0; i < children->length; ++i)
		{
			const GumboNode* child = static_cast<const GumboNode*>(children->data[i]);
			std::vector<const GumboNode*> childResult = findNodesRecursive(child, predicate);

			result.insert(std::end(result), std::begin(childResult), std::end(childResult));
		}
	}

	return result;
}

template <class UnaryPredicate, class MapFunc>
static auto findNodesAndGetResult(const GumboNode* parentNode, UnaryPredicate predicate, MapFunc mapFunc)
{
	std::vector<const GumboNode*> nodes = findNodesRecursive(parentNode, predicate);
	std::vector<decltype(mapFunc(parentNode))> result;

	for (const GumboNode* node : nodes)
	{
		result.push_back(mapFunc(node));
	}

	return result;
}

bool checkAttribute(const GumboNode* node, const char* attribute, const char* expectedValue) noexcept
{
	const GumboAttribute* attr = gumbo_get_attribute(&node->v.element.attributes, attribute);
	return attr && (strlen(expectedValue) == 0 || QString(attr->value).toLower().trimmed() == expectedValue);
}

}

namespace CrawlerEngine
{

GumboHtmlParser::GumboHtmlParser() 
	: m_gumboOptions(&kGumboDefaultOptions)
	, m_gumboOutput(nullptr)
	, m_rootNode(nullptr)
{
}

GumboHtmlParser::GumboHtmlParser(const GumboOptions* options, const QByteArray& htmlPage)
	: m_gumboOptions(options)
	, m_gumboOutput(gumbo_parse_with_options(options, htmlPage.data(), htmlPage.size()))
	, m_htmlPage(htmlPage)
	, m_rootNode(m_gumboOutput->root)
{
}

GumboHtmlParser::~GumboHtmlParser()
{
	gumbo_destroy_output(m_gumboOptions, m_gumboOutput);
}

void GumboHtmlParser::parseHtmlPage(const QByteArray& htmlPage, const ResponseHeaders& headers)
{
	if (m_gumboOutput)
	{
		gumbo_destroy_output(m_gumboOptions, m_gumboOutput);
	}

	m_gumboOutput = gumbo_parse_with_options(&kGumboDefaultOptions, htmlPage.data(), htmlPage.size());
	m_htmlPage = htmlPage;
	m_rootNode = GumboHtmlNode(m_gumboOutput->root);

	// we need double parsing because we need firstly decode html page
	// this need in order to we'll be able to extract right data
	m_htmlPage = decodeHtmlPage(headers);

	gumbo_destroy_output(m_gumboOptions, m_gumboOutput);
	m_gumboOutput = gumbo_parse_with_options(&kGumboDefaultOptions, m_htmlPage.data(), m_htmlPage.size());
	m_rootNode = GumboHtmlNode(m_gumboOutput->root);
}

QByteArray GumboHtmlParser::identifyHtmlPageContentType() const
{
	IHtmlNodeCountedPtr head = m_rootNode.firstMatchSubNode(IHtmlNode::TagIdHead);

	DEBUG_ASSERT(head->type() == IHtmlNode::NodeTypeElement && head->tagId() == IHtmlNode::TagIdHead);

	std::vector<IHtmlNodeCountedPtr> metaTags = head->matchSubNodes(IHtmlNode::TagIdMeta);

	for (unsigned i = 0; i < metaTags.size(); ++i)
	{
		if (!metaTags[i]->hasAttribute("content"))
		{
			continue;
		}

		const QString contentAttribute = metaTags[i]->attribute("content");

		if (metaTags[i]->hasAttribute("http-equiv"))
		{
			const QString attributeValue = metaTags[i]->attribute("http-equiv").toLower();

			if (attributeValue == "content-type")
			{
				return contentAttribute.toLatin1();
			}
		}
	}

	return QByteArray();
}

QByteArray GumboHtmlParser::decodeHtmlPage(const ResponseHeaders& headers)
{
	const QByteArray contentType = identifyHtmlPageContentType();
	const QByteArray charsetFromHtmlPage = contentType.right(contentType.size() - contentType.lastIndexOf("=") - 1);
	const std::vector<QString> contentTypeValues = headers.valueOf("content-type");

	if (!contentTypeValues.empty())
	{
		for (const QString& contentTypeValue : contentTypeValues)
		{
			const QByteArray charsetFromHttpResponse = contentTypeValue.right(contentTypeValue.size() - contentTypeValue.lastIndexOf("=") - 1).toLatin1();
			const QTextCodec* codecForCharset = QTextCodec::codecForName(charsetFromHttpResponse);

			if (!codecForCharset)
			{
				continue;
			}

			m_htmlPage = codecForCharset->toUnicode(m_htmlPage).toStdString().data();

			return m_htmlPage;
		}
	}

	if (!charsetFromHtmlPage.isEmpty())
	{
		QTextCodec* codecForCharset = QTextCodec::codecForName(charsetFromHtmlPage);

		if (codecForCharset)
		{
			m_htmlPage = codecForCharset->toUnicode(m_htmlPage.data()).toStdString().data();
		}
		else
		{
			ERRLOG << "Cannot find QTextCodec for page encoding";
			ERRLOG << "content-type:" << contentType;
			ERRLOG << "charset:" << charsetFromHtmlPage;
		}
	}
	else
	{
		QTextCodec* codecForHtml = QTextCodec::codecForHtml(m_htmlPage);

		if (codecForHtml)
		{
			m_htmlPage = codecForHtml->toUnicode(m_htmlPage).toStdString().data();
		}
		else
		{
			ERRLOG << "Cannot identify page encoding";
		}
	}

	return m_htmlPage;
}

std::vector<LinkInfo> GumboHtmlParser::pageUrlList(bool httpOrHttpsOnly) const
{
	const auto predicate = [httpOrHttpsOnly](const GumboNode* node)
	{
		bool result = node &&
			node->type == GUMBO_NODE_ELEMENT &&
			node->v.element.tag == GUMBO_TAG_A &&
			gumbo_get_attribute(&node->v.element.attributes, "href");

		if (!result || !httpOrHttpsOnly)
		{
			return result;
		}

		GumboAttribute* href = gumbo_get_attribute(&node->v.element.attributes, "href");
		QString hrefVal = href->value;

		return PageParserHelpers::isHttpOrHttpsScheme(Url(hrefVal));
	};

	const auto resultGetter = [](const GumboNode* node)
	{
		const QRegularExpression regExp("[\\n\\t]+");

		const GumboAttribute* href = gumbo_get_attribute(&node->v.element.attributes, "href");
		const GumboAttribute* rel = gumbo_get_attribute(&node->v.element.attributes, "rel");

		LinkParameter linkParam = LinkParameter::DofollowParameter;

		if (rel != nullptr && std::strstr(rel->value, "nofollow") != nullptr)
		{
			linkParam = LinkParameter::NofollowParameter;
		}

		const QString altOrTitle(GumboHtmlNode(const_cast<GumboNode*>(node)).text());
		const bool dataResource = QByteArray(href->value).startsWith("data:");
		const Url url = QString(href->value).trimmed().remove(regExp);

		return LinkInfo{ url, linkParam, altOrTitle, dataResource, ResourceSource::SourceTagA };
	};

	std::vector<LinkInfo> result = findNodesAndGetResult(m_gumboOutput->root, predicate, resultGetter);

	const LinkInfo canonical = getLinkRelUrl(m_gumboOutput->root, "canonical", ResourceSource::SourceTagLinkRelCanonical);

	if (canonical.resourceSource != ResourceSource::SourceInvalid)
	{
		result.push_back(canonical);
	}

	const LinkInfo next = getLinkRelUrl(m_gumboOutput->root, "next", ResourceSource::SourceTagLinkRelNext);

	if (next.resourceSource != ResourceSource::SourceInvalid)
	{
		result.push_back(next);
	}

	const LinkInfo prev = getLinkRelUrl(m_gumboOutput->root, "prev", ResourceSource::SourceTagLinkRelPrev);

	if (prev.resourceSource != ResourceSource::SourceInvalid)
	{
		result.push_back(prev);
	}

	return result;
}

LinkInfo GumboHtmlParser::getLinkRelUrl(const GumboNode* node, const char* relValue, ResourceSource source) const
{
	const auto cond = [relValue](const GumboNode* node)
	{
		bool result = node &&
			node->type == GUMBO_NODE_ELEMENT &&
			node->v.element.tag == GUMBO_TAG_LINK &&
			checkAttribute(node, "rel", relValue) &&
			gumbo_get_attribute(&node->v.element.attributes, "href");
		return result;
	};

	const auto res = [source](const GumboNode* node)
	{
		const GumboAttribute* href = gumbo_get_attribute(&node->v.element.attributes, "href");
		const LinkParameter linkParam = LinkParameter::DofollowParameter;
		const bool dataResource = QByteArray(href->value).startsWith("data:");

		return LinkInfo{ Url(href->value), linkParam, QString(), dataResource, source };
	};

	std::vector<LinkInfo> result = findNodesAndGetResult(node, cond, res);

	if (!result.empty())
	{
		return result[0];
	}

	return LinkInfo{ Url(), LinkParameter::DofollowParameter, QString(), false, ResourceSource::SourceInvalid };
}

IHtmlNodeCountedPtr GumboHtmlParser::firstMatchNode(IHtmlNode::TagId tagId) const
{
	return m_rootNode.firstMatchSubNode(tagId);
}

std::vector<IHtmlNodeCountedPtr> GumboHtmlParser::matchNodes(IHtmlNode::TagId tagId) const
{
	return m_rootNode.matchSubNodes(tagId);
}

std::vector<IHtmlNodeCountedPtr> GumboHtmlParser::matchNodesInDepth(IHtmlNode::TagId tagId) const
{
	return m_rootNode.matchSubNodesInDepth(tagId);
}

std::vector<IHtmlNodeCountedPtr> GumboHtmlParser::matchNodesInDepth(const std::function<bool(const IHtmlNode&)>& predicate) const
{
	return m_rootNode.matchSubNodesInDepth(predicate);
}

IHtmlNodeCountedPtr GumboHtmlParser::findNodeWithAttributeValue(IHtmlNode::TagId tagId, std::pair<const char*, const char*> expectedAttributes) const
{
	return m_rootNode.childNodeByAttributeValue(tagId, expectedAttributes);
}

IHtmlNodeCountedPtr GumboHtmlParser::findNodeWithAttributesValues(IHtmlNode::TagId tagId, const std::map<const char*, const char*>& expectedAttributes) const
{
	return m_rootNode.childNodeByAttributesValues(tagId, expectedAttributes);
}

QByteArray GumboHtmlParser::htmlPageContent() const
{
	return m_htmlPage;
}

}