#include "gumbo_parsing_helpers.h"
#include "page_parser_helpers.h"

namespace WebCrawler
{

GumboOutputCreatorDestroyerGuard::GumboOutputCreatorDestroyerGuard(const GumboOptions* options, const QByteArray& htmlPage)
	: m_gumboOptions(options)
	, m_gumboOutput(gumbo_parse_with_options(options, htmlPage.data(), htmlPage.size()))
{
}

GumboOutputCreatorDestroyerGuard::~GumboOutputCreatorDestroyerGuard()
{
	gumbo_destroy_output(m_gumboOptions, m_gumboOutput);
}

const GumboOutput* GumboOutputCreatorDestroyerGuard::output() const noexcept
{
	return m_gumboOutput;
}

GumboOutput* GumboOutputCreatorDestroyerGuard::output() noexcept
{
	return m_gumboOutput;
}


GumboNode* GumboParsingHelpers::findSubNode(const GumboNode* node, GumboTag tag, unsigned startIndexWhithinParent) noexcept
{
	DEBUG_ASSERT(node->type == GUMBO_NODE_ELEMENT || node->type == GUMBO_NODE_DOCUMENT);

	const GumboVector* children = &node->v.element.children;
	GumboNode* searchingNode = nullptr;

	DEBUG_ASSERT(startIndexWhithinParent <= children->length);

	for (unsigned i = startIndexWhithinParent; i < children->length; ++i)
	{
		GumboNode* child = static_cast<GumboNode*>(children->data[i]);

		if (child->type == GUMBO_NODE_ELEMENT && child->v.element.tag == tag)
		{
			searchingNode = child;
			break;
		}
	}

	return searchingNode;
}

QByteArray GumboParsingHelpers::nodeText(const GumboNode* node) noexcept
{
	if (node->type == GUMBO_NODE_TEXT)
	{
		return node->v.text.text;
	}

	if (node->v.element.children.length > 0)
	{
		GumboNode* textNode = static_cast<GumboNode*>(node->v.element.children.data[0]);
		if (textNode->type == GUMBO_NODE_TEXT)
		{
			return textNode->v.text.text;
		}
	}
	
	return QByteArray();
}

std::vector<GumboNode*> GumboParsingHelpers::subNodes(const GumboNode* node, GumboTag tag) noexcept
{
	DEBUG_ASSERT(node->type == GUMBO_NODE_ELEMENT || node->type == GUMBO_NODE_DOCUMENT);

	const GumboVector* children = &node->v.element.children;
	std::vector<GumboNode*> nodes;

	for (unsigned i = 0; i < children->length; ++i)
	{
		GumboNode* child = static_cast<GumboNode*>(children->data[i]);

		if (child->type == GUMBO_NODE_ELEMENT && child->v.element.tag == tag)
		{
			nodes.push_back(child);
		}
	}

	return nodes;
}

unsigned GumboParsingHelpers::countChildren(const GumboNode* node, GumboTag tag) noexcept
{
	DEBUG_ASSERT(node->type == GUMBO_NODE_ELEMENT || node->type == GUMBO_NODE_DOCUMENT);

	const GumboVector* children = &node->v.element.children;
	unsigned counter = 0;

	for (unsigned i = 0; i < children->length; ++i)
	{
		const GumboNode* child = static_cast<const GumboNode*>(children->data[i]);

		if (child->type == GUMBO_NODE_ELEMENT && child->v.element.tag == tag)
		{
			++counter;
		}
	}

	return counter;
}

QByteArray GumboParsingHelpers::cutAllTagsFromNode(const GumboNode* node) noexcept
{
	QByteArray text;
	cutAllTagsFromNodeHelper(node, text);

	return text;
}


QByteArray GumboParsingHelpers::identifyHtmlPageContentType(const QByteArray& htmlPage) noexcept
{
	GumboOutputCreatorDestroyerGuard gumboOutput(&kGumboDefaultOptions, htmlPage);

	GumboNode* head = GumboParsingHelpers::findSubNode(gumboOutput.output()->root, GUMBO_TAG_HEAD);
	DEBUG_ASSERT(head->type == GUMBO_NODE_ELEMENT && head->v.element.tag == GUMBO_TAG_HEAD);

	std::vector<GumboNode*> metaTags = GumboParsingHelpers::subNodes(head, GUMBO_TAG_META);

	for (unsigned i = 0; i < metaTags.size(); ++i)
	{
		const GumboAttribute* contentAttribute = gumbo_get_attribute(&metaTags[i]->v.element.attributes, "content");

		if (!contentAttribute)
		{
			continue;
		}

		const GumboAttribute* metaHttpEquivAttribute = gumbo_get_attribute(&metaTags[i]->v.element.attributes, "http-equiv");

		if (metaHttpEquivAttribute)
		{
			const QString attributeValue = QString(metaHttpEquivAttribute->value).toLower();

			if (attributeValue == "content-type")
			{
				return contentAttribute->value;
			}
		}
	}

	return QByteArray();
}


QByteArray GumboParsingHelpers::decodeHtmlPage(const QByteArray& htmlPage) noexcept
{
	QByteArray decodedHtmlPage = htmlPage;
	QByteArray contentType = GumboParsingHelpers::identifyHtmlPageContentType(htmlPage);

	QByteArray charset = contentType.right(contentType.size() - contentType.lastIndexOf("=") - 1);

	if (charset.isEmpty())
	{
		QTextCodec* codecForHtml = QTextCodec::codecForHtml(decodedHtmlPage.data());

		if (codecForHtml)
		{
			decodedHtmlPage = codecForHtml->toUnicode(decodedHtmlPage).toStdString().data();
		}
		else
		{
			ERRORLOG << "Cannot identify page encoding";
		}
	}
	else
	{
		QTextCodec* codecForCharset = QTextCodec::codecForName(charset);

		if (codecForCharset)
		{
			decodedHtmlPage = codecForCharset->toUnicode(decodedHtmlPage.data()).toStdString().data();
		}
		else
		{
			ERRORLOG << "Cannot find QTextCodec for page encoding";
			ERRORLOG << "content-type:" << contentType;
			ERRORLOG << "charset:" << charset;
		}
	}

	return decodedHtmlPage;
}

std::vector<LinkInfo> GumboParsingHelpers::parsePageUrlList(const GumboNode* node, bool httpOrHttpsOnly) noexcept
{
	auto cond = [httpOrHttpsOnly](const GumboNode* node)
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
		
		return PageParserHelpers::isHttpOrHttpsScheme(hrefVal);
	};

	auto res = [](const GumboNode* node)
	{
		GumboAttribute* href = href = gumbo_get_attribute(&node->v.element.attributes, "href");
		GumboAttribute* rel = gumbo_get_attribute(&node->v.element.attributes, "rel");

		LinkParameter linkParam = LinkParameter::DofollowParameter;

		if (rel != nullptr && rel->value == "nofollow")
		{
			linkParam = LinkParameter::NofollowParameter;
		}

		QString altOrTitle = QString(nodeText(node));

		return LinkInfo{ QUrl(href->value), linkParam, altOrTitle };
	};

	const std::vector<LinkInfo> result = findNodesAndGetResult(node, cond, res);

	return result;
}

const GumboNode* GumboParsingHelpers::findChildNode(const GumboNode* node, GumboTag expectedTag, std::map<const char*, const char*> expectedAttributes) noexcept
{
	if (!node || node->type != GUMBO_NODE_ELEMENT)
	{
		return nullptr;
	}

	const GumboVector* children = &node->v.element.children;

	for (unsigned int i = 0; i < children->length; ++i)
	{
		const GumboNode* child = static_cast<const GumboNode*>(children->data[i]);

		if (!child || child->type != GUMBO_NODE_ELEMENT || child->v.element.tag != expectedTag)
		{
			continue;
		}

		bool success = true;

		for (auto attr : expectedAttributes)
		{
			GumboAttribute* gumboAttr = gumbo_get_attribute(&child->v.element.attributes, attr.first);
			success = success && gumboAttr && (strlen(attr.second) == 0 || QString(gumboAttr->value).toLower() == attr.second);

			if (!success)
			{
				break;
			}
		}

		if (success)
		{
			return child;
		}
	}

	return nullptr;
}

const GumboNode* GumboParsingHelpers::findChildNode(const GumboNode* node, GumboTag expectedTag, std::pair<const char*, const char*> expectedAttributes) noexcept
{
	return findChildNode(node, expectedTag, std::map<const char*, const char*>{ expectedAttributes });
}

bool GumboParsingHelpers::checkAttribute(const GumboNode* node, const char* attribute, const char* expectedValue) noexcept
{
	const GumboAttribute* attr = gumbo_get_attribute(&node->v.element.attributes, attribute);
	return attr && (strlen(expectedValue) == 0 || QString(attr->value).toLower().trimmed() == expectedValue);
}

void GumboParsingHelpers::cutAllTagsFromNodeHelper(const GumboNode* node, QByteArray& result) noexcept
{
	if (!node)
	{
		return;
	}

	if (node->type == GUMBO_NODE_TEXT)
	{
		result += QByteArray(nodeText(node)) + " ";
		return;
	}

	const GumboVector* children = &node->v.element.children;

	for (unsigned i = 0; i < children->length; ++i)
	{
		const GumboNode* child = static_cast<const GumboNode*>(children->data[i]);

		if (child->type == GUMBO_NODE_ELEMENT || child->type == GUMBO_NODE_TEXT)
		{
			cutAllTagsFromNodeHelper(child, result);
		}
	}
}

}