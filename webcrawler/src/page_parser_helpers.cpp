#include "page_parser_helpers.h"

namespace WebCrawler
{

QUrl PageParserHelpers::resolveRelativeUrl(const QUrl& relativeUrl, const QUrl& baseUrl)
{
	QUrl result = relativeUrl;

	if (result.toDisplayString().startsWith("//"))
	{
		return QString("http:") + result.toDisplayString();
	}

	//
	// see: https://tools.ietf.org/html/rfc1808#section-4
	//

	DEBUG_ASSERT(!baseUrl.isRelative() && "Base URL always MUST BE an absolute URL!!!");


	if (!result.isRelative())
	{
		DEBUGLOG << "Passed non-relative url:" << result.toDisplayString();

		return result;
	}

	result.setScheme(baseUrl.scheme());
	result.setHost(baseUrl.host());

	QString pathWithoutFile = baseUrl.path();
	int lastSlashIndex = pathWithoutFile.lastIndexOf("/");

	if (result.path().startsWith("/"))
	{
		//
		// Just exit from function because we already make it absolute
		//
		return result;
	}
	else
	{
		//
		// Make path starts with slash for ensure valid QUrl behavior
		//
		result.setPath("/" + result.path());
	}

	if (result.path().isEmpty())
	{
		result.setPath(pathWithoutFile.left(lastSlashIndex + 1));
	}
	else
	{
		result.setPath(pathWithoutFile.left(lastSlashIndex) + result.path());
	}

	QString fixedPath = result.path();
	if (fixedPath.contains("/./"))
	{
		fixedPath = fixedPath.replace("/./", "/");
	}
	
	while (fixedPath.contains("/../"))
	{
		int doublePointsPos = fixedPath.indexOf("/../");
		int prevSlashPos = fixedPath.left(doublePointsPos).lastIndexOf("/");
		if (prevSlashPos == -1)
		{
			WARNINGLOG << "Can't resolve absolute path: " << fixedPath;
			break;
		}
		
		fixedPath = fixedPath.left(prevSlashPos + 1) + fixedPath.right(fixedPath.size() - doublePointsPos - 4);
	}

	result.setPath(fixedPath);

	return result;
}

std::vector<QUrl> PageParserHelpers::resolveUrlList(const QUrl& baseUrl, 
	const std::vector<QUrl>& urlList) noexcept
{
	std::vector<QUrl> result;

	for (const QUrl& url : urlList)
	{
		result.push_back(resolveUrl(baseUrl, url));
	}

	return result;
}

std::vector<LinkInfo> PageParserHelpers::resolveUrlList(const QUrl& baseUrl,
	const std::vector<LinkInfo>& linkList) noexcept
{
	std::vector<LinkInfo> result = linkList;

	for (LinkInfo& link : result)
	{
		link.url = resolveUrl(baseUrl, link.url);
	}

	return result;
}

QUrl PageParserHelpers::resolveUrl(const QUrl& baseUrl, const QUrl& url) noexcept
{
	QUrl result = url;

	if (result.isRelative())
	{
		result = resolveRelativeUrl(result, baseUrl);
	}

	result.setFragment(QString());

	return result;
}

bool PageParserHelpers::isUrlExternal(const QUrl& baseUrl, const QUrl& url) noexcept
{
	// TODO: improve
	QString baseUrlHost = baseUrl.host().toLower();
	QString urlHost = url.host().toLower();
	
	const bool isUrlInternal =  baseUrlHost == urlHost || 
		(urlHost.endsWith("www." + baseUrlHost)) ||
		(baseUrlHost.endsWith("www." + urlHost));

	// TODO: what if both urls are sub-domains?

	return !isUrlInternal;
}

bool PageParserHelpers::isHtmlContentType(const QString& contentType) noexcept
{
	return contentType.startsWith("text/html") ||
		contentType.startsWith("text/xhtml") ||
		contentType.startsWith("application/xhtml") ||
		contentType.isEmpty();
}

bool PageParserHelpers::isHttpOrHttpsScheme(const QString& urlStr) noexcept
{
	static QRegularExpression expr("^\\w{0,10}:");

	if (expr.match(urlStr).hasMatch() && !urlStr.startsWith("https://") && !urlStr.startsWith("http://"))
	{
		return false;
	}

	return true;

}

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


GumboNode* PageParserHelpers::findSubNode(const GumboNode* node, GumboTag tag, unsigned startIndexWhithinParent) noexcept
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

QByteArray PageParserHelpers::nodeText(const GumboNode* node) noexcept
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

std::vector<GumboNode*> PageParserHelpers::subNodes(const GumboNode* node, GumboTag tag) noexcept
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

unsigned PageParserHelpers::countChildren(const GumboNode* node, GumboTag tag) noexcept
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

QByteArray PageParserHelpers::cutAllTagsFromNode(const GumboNode* node) noexcept
{
	QByteArray text;
	cutAllTagsFromNodeHelper(node, text);

	return text;
}


QByteArray PageParserHelpers::identifyHtmlPageContentType(const QByteArray& htmlPage) noexcept
{
	GumboOutputCreatorDestroyerGuard gumboOutput(&kGumboDefaultOptions, htmlPage);

	GumboNode* head = PageParserHelpers::findSubNode(gumboOutput.output()->root, GUMBO_TAG_HEAD);
	DEBUG_ASSERT(head->type == GUMBO_NODE_ELEMENT && head->v.element.tag == GUMBO_TAG_HEAD);

	std::vector<GumboNode*> metaTags = PageParserHelpers::subNodes(head, GUMBO_TAG_META);

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


QByteArray PageParserHelpers::decodeHtmlPage(const QByteArray& htmlPage) noexcept
{
	QByteArray decodedHtmlPage = htmlPage;
	QByteArray contentType = PageParserHelpers::identifyHtmlPageContentType(htmlPage);

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

std::vector<LinkInfo> PageParserHelpers::parsePageUrlList(const GumboNode* node, bool httpOrHttpsOnly) noexcept
{
	const auto cond = [httpOrHttpsOnly](const GumboNode* node)
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

	const auto res = [](const GumboNode* node)
	{
		const GumboAttribute* href = href = gumbo_get_attribute(&node->v.element.attributes, "href");
		const GumboAttribute* rel = gumbo_get_attribute(&node->v.element.attributes, "rel");

		LinkParameter linkParam = LinkParameter::DofollowParameter;

		if (rel != nullptr && std::strstr(rel->value, "nofollow") != nullptr)
		{
			linkParam = LinkParameter::NofollowParameter;
		}

		const QString altOrTitle(nodeText(node));
		const bool dataResource = QByteArray(href->value).startsWith("data:");

		return LinkInfo{ QUrl(href->value), linkParam, altOrTitle, dataResource };
	};

	const std::vector<LinkInfo> result = findNodesAndGetResult(node, cond, res);

	return result;
}

const GumboNode* PageParserHelpers::findChildNode(const GumboNode* node, GumboTag expectedTag, std::map<const char*, const char*> expectedAttributes) noexcept
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

		for (const auto attr : expectedAttributes)
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

const GumboNode* PageParserHelpers::findChildNode(const GumboNode* node, GumboTag expectedTag, std::pair<const char*, const char*> expectedAttributes) noexcept
{
	return findChildNode(node, expectedTag, std::map<const char*, const char*>{ expectedAttributes });
}

bool PageParserHelpers::checkAttribute(const GumboNode* node, const char* attribute, const char* expectedValue) noexcept
{
	const GumboAttribute* attr = gumbo_get_attribute(&node->v.element.attributes, attribute);
	return attr && (strlen(expectedValue) == 0 || QString(attr->value).toLower().trimmed() == expectedValue);
}

void PageParserHelpers::cutAllTagsFromNodeHelper(const GumboNode* node, QByteArray& result) noexcept
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