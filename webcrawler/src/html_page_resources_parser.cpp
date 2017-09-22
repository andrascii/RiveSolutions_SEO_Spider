#include "html_page_resources_parser.h"
#include "gumbo_parsing_helpers.h"
#include "page_raw_parser_helpers.h"

namespace WebCrawler
{

class ResourcesCacheGuard
{
public:
	ResourcesCacheGuard(QSet<QString>& cache, const std::deque<PageRawResource>& resources)
		: m_cache(cache)
	{
		for (const PageRawResource& resource : resources)
		{
			m_cache.insert(resource.resourceLink.url.toDisplayString());
		}
	}

	~ResourcesCacheGuard()
	{
		m_cache.clear();
	}

private:
	QSet<QString>& m_cache;
};

void HtmlPageResourcesParser::parse(GumboOutput* output, PageRawPtr& pageRaw) noexcept
{
	ResourcesCacheGuard cacheGurad(m_resourcesCache, pageRaw->allResourcesOnPage);

	parseResourceType(output, pageRaw);

	if (pageRaw->resourceType != ResourceType::ResourceHtml)
	{
		return;
	}

	parseHtmlResources(output, pageRaw);
	parseJavaScriptResources(output, pageRaw);
	parseStyleSheetResources(output, pageRaw);
	parseImageResources(output, pageRaw);
	parseVideoResources(output, pageRaw);
	parseFlashResourcesV1(output, pageRaw);
	parseFlashResourcesV2(output, pageRaw);
	parseFlashResourcesV3(output, pageRaw);
}

void HtmlPageResourcesParser::parseResourceType(GumboOutput* output, PageRawPtr& pageRaw) noexcept
{
	if (pageRaw->contentType.contains("javascript"))
	{
		pageRaw->resourceType = ResourceType::ResourceJavaScript;
		return;
	}

	if (pageRaw->contentType.startsWith("image/"))
	{
		pageRaw->resourceType = ResourceType::ResourceImage;
		return;
	}

	if (PageRawParserHelpers::isHtmlContentType(pageRaw->contentType))
	{
		pageRaw->resourceType = ResourceType::ResourceHtml;
		return;
	}

	WARNINGLOG << "Unknown resource type: " << pageRaw->contentType;

	pageRaw->resourceType = ResourceType::ResourceOther;
}

void HtmlPageResourcesParser::parseHtmlResources(GumboOutput* output, PageRawPtr& pageRaw) noexcept
{
	std::vector<OnPageUrl> links = GumboParsingHelpers::parsePageUrlList(output->root);

	std::vector<QUrl> unresolvedUrls;
	for (const OnPageUrl& link : links)
	{
		unresolvedUrls.push_back(link.url);
	}

	std::vector<QUrl> resolvedUrls = PageRawParserHelpers::resolveUrlList(pageRaw->url, unresolvedUrls);

	for (size_t i = 0; i < resolvedUrls.size(); ++i)
	{
		const QUrl& url = resolvedUrls[i];
		if (!resourceExists(url))
		{
			ResourceType resourceType = PageRawParserHelpers::isHttpOrHttpsScheme(url.toDisplayString()) ?
				ResourceType::ResourceHtml : ResourceType::ResourceOther;

			pageRaw->allResourcesOnPage.push_back(PageRawResource{ resourceType, OnPageUrl{ url, links[i].linkParameter } });
		}
	}
}

void HtmlPageResourcesParser::parseJavaScriptResources(GumboOutput* output, PageRawPtr& pageRaw) noexcept
{
	auto cond = [](const GumboNode* node)
	{
		return node &&
			node->type == GUMBO_NODE_ELEMENT &&
			node->v.element.tag == GUMBO_TAG_SCRIPT &&
			gumbo_get_attribute(&node->v.element.attributes, "src");
	};

	auto res = [](const GumboNode* node)
	{
		GumboAttribute* src = gumbo_get_attribute(&node->v.element.attributes, "src");
		return QUrl(src->value);
	};

	std::vector<QUrl> urls = GumboParsingHelpers::findNodesAndGetResult(output->root, cond, res);
	std::vector<QUrl> resolvedUrls = PageRawParserHelpers::resolveUrlList(pageRaw->url, urls);

	for (const QUrl& url : resolvedUrls)
	{
		if (!resourceExists(url))
		{
			pageRaw->allResourcesOnPage.push_back(PageRawResource{ ResourceType::ResourceJavaScript, OnPageUrl{ url, UrlParameter::UnknownParameter } });
		}
		
	}
}

void HtmlPageResourcesParser::parseStyleSheetResources(GumboOutput* output, PageRawPtr& pageRaw) noexcept
{
	auto cond = [](const GumboNode* node)
	{
		return node &&
			node->type == GUMBO_NODE_ELEMENT &&
			node->v.element.tag == GUMBO_TAG_LINK &&
			gumbo_get_attribute(&node->v.element.attributes, "href") &&
			GumboParsingHelpers::checkAttribute(node, "rel", "stylesheet");
	};

	auto res = [](const GumboNode* node)
	{
		GumboAttribute* href = gumbo_get_attribute(&node->v.element.attributes, "href");
		return QUrl(href->value);
	};

	std::vector<QUrl> urls = GumboParsingHelpers::findNodesAndGetResult(output->root, cond, res);
	std::vector<QUrl> resolvedUrls = PageRawParserHelpers::resolveUrlList(pageRaw->url, urls);

	for (const QUrl& url : resolvedUrls)
	{
		if (!resourceExists(url))
		{
			pageRaw->allResourcesOnPage.push_back(PageRawResource{ ResourceType::ResourceStyleSheet, OnPageUrl{ url, UrlParameter::UnknownParameter } });
		}
	}
}

void HtmlPageResourcesParser::parseImageResources(GumboOutput* output, PageRawPtr& pageRaw) noexcept
{
	auto cond = [](const GumboNode* node)
	{
		return node &&
			node->type == GUMBO_NODE_ELEMENT &&
			node->v.element.tag == GUMBO_TAG_IMG &&
			gumbo_get_attribute(&node->v.element.attributes, "src");
	};

	auto res = [](const GumboNode* node)
	{
		GumboAttribute* src = gumbo_get_attribute(&node->v.element.attributes, "src");
		return QUrl(src->value);
	};

	std::vector<QUrl> urls = GumboParsingHelpers::findNodesAndGetResult(output->root, cond, res);
	std::vector<QUrl> resolvedUrls = PageRawParserHelpers::resolveUrlList(pageRaw->url, urls);

	for (const QUrl& url : resolvedUrls)
	{
		if (!resourceExists(url))
		{
			pageRaw->allResourcesOnPage.push_back(PageRawResource{ ResourceType::ResourceImage, OnPageUrl{ url, UrlParameter::UnknownParameter } });
		}
	}
}

void HtmlPageResourcesParser::parseVideoResources(GumboOutput* output, PageRawPtr& pageRaw) noexcept
{
	auto cond = [](const GumboNode* node)
	{
		return node &&
			node->type == GUMBO_NODE_ELEMENT &&
			node->v.element.tag == GUMBO_TAG_VIDEO &&
			!!GumboParsingHelpers::findChildNode(node, GUMBO_TAG_SOURCE, std::make_pair("src", ""));
	};

	auto res = [](const GumboNode* node)
	{
		const GumboNode* child = GumboParsingHelpers::findChildNode(node, GUMBO_TAG_SOURCE, std::make_pair("src", ""));
		GumboAttribute* src = gumbo_get_attribute(&child->v.element.attributes, "src");
		return QUrl(src->value);
	};

	std::vector<QUrl> urls = GumboParsingHelpers::findNodesAndGetResult(output->root, cond, res);
	std::vector<QUrl> resolvedUrls = PageRawParserHelpers::resolveUrlList(pageRaw->url, urls);

	for (const QUrl& url : resolvedUrls)
	{
		if (!resourceExists(url))
		{
			pageRaw->allResourcesOnPage.push_back(PageRawResource{ ResourceType::ResourceVideo, OnPageUrl{ url, UrlParameter::UnknownParameter } });
		}
	}
}

void HtmlPageResourcesParser::parseFlashResourcesV1(GumboOutput* output, PageRawPtr& pageRaw) noexcept
{
	//<embed type="application/x-shockwave-flash" src="myContent.swf" width="300" height="120" pluginspage="http://www.adobe.com/go/getflashplayer" / >
	//<noembed>Alternative content</noembed>

	auto cond = [](const GumboNode* node)
	{
		return node &&
			node->type == GUMBO_NODE_ELEMENT &&
			node->v.element.tag == GUMBO_TAG_EMBED &&
			gumbo_get_attribute(&node->v.element.attributes, "src") &&
			GumboParsingHelpers::checkAttribute(node, "type", "application/x-shockwave-flash");
	};

	auto res = [](const GumboNode* node)
	{
		GumboAttribute* src = gumbo_get_attribute(&node->v.element.attributes, "src");
		return QUrl(src->value);
	};

	std::vector<QUrl> urls = GumboParsingHelpers::findNodesAndGetResult(output->root, cond, res);
	std::vector<QUrl> resolvedUrls = PageRawParserHelpers::resolveUrlList(pageRaw->url, urls);

	for (const QUrl& url : resolvedUrls)
	{
		if (!resourceExists(url))
		{
			pageRaw->allResourcesOnPage.push_back(PageRawResource{ ResourceType::ResourceFlash, OnPageUrl{ url, UrlParameter::UnknownParameter } });
		}
	}
}

void HtmlPageResourcesParser::parseFlashResourcesV2(GumboOutput* output, PageRawPtr& pageRaw) noexcept
{
	// 	<object type="application/x-shockwave-flash" data="myContent.swf" width="300" height="120">
	// 	<p>Alternative content</p>
	// 	</object>

	auto cond = [](const GumboNode* node)
	{
		return node &&
			node->type == GUMBO_NODE_ELEMENT &&
			node->v.element.tag == GUMBO_TAG_OBJECT &&
			gumbo_get_attribute(&node->v.element.attributes, "data") &&
			GumboParsingHelpers::checkAttribute(node, "type", "application/x-shockwave-flash");
	};

	auto res = [](const GumboNode* node)
	{
		GumboAttribute* src = gumbo_get_attribute(&node->v.element.attributes, "data");
		return QUrl(src->value);
	};

	std::vector<QUrl> urls = GumboParsingHelpers::findNodesAndGetResult(output->root, cond, res);
	std::vector<QUrl> resolvedUrls = PageRawParserHelpers::resolveUrlList(pageRaw->url, urls);

	for (const QUrl& url : resolvedUrls)
	{
		if (!resourceExists(url))
		{
			pageRaw->allResourcesOnPage.push_back(PageRawResource{ ResourceType::ResourceFlash, OnPageUrl{ url, UrlParameter::UnknownParameter } });
		}
	}
}

void HtmlPageResourcesParser::parseFlashResourcesV3(GumboOutput* output, PageRawPtr& pageRaw) noexcept
{
	// 	<object classid="clsid:D27CDB6E-AE6D-11cf-96B8-444553540000" width="300" height="120">
	// 	<param name="movie" value="myContent.swf" />
	// 	<p>Alternative content</p>
	// 	</object>

	auto cond = [](const GumboNode* node)
	{
		return node &&
			node->type == GUMBO_NODE_ELEMENT &&
			node->v.element.tag == GUMBO_TAG_OBJECT &&
			GumboParsingHelpers::checkAttribute(node, "classid", "clsid:D27CDB6E-AE6D-11cf-96B8-444553540000") &&
			(!!GumboParsingHelpers::findChildNode(node, GUMBO_TAG_PARAM, std::make_pair("movie", "")) ||
				!!GumboParsingHelpers::findChildNode(node, GUMBO_TAG_PARAM, std::make_pair("src", "")));

	};

	auto res = [](const GumboNode* node)
	{
		const GumboNode* childNode = GumboParsingHelpers::findChildNode(node, GUMBO_TAG_PARAM, std::make_pair("movie", ""));
		if (childNode)
		{
			GumboAttribute* movie = gumbo_get_attribute(&childNode->v.element.attributes, "movie");
			return QUrl(movie->value);
		}
			
		childNode = GumboParsingHelpers::findChildNode(node, GUMBO_TAG_PARAM, std::make_pair("src", ""));

		GumboAttribute* src = gumbo_get_attribute(&childNode->v.element.attributes, "src");
		return QUrl(src->value);
	};

	std::vector<QUrl> urls = GumboParsingHelpers::findNodesAndGetResult(output->root, cond, res);
	std::vector<QUrl> resolvedUrls = PageRawParserHelpers::resolveUrlList(pageRaw->url, urls);

	for (const QUrl& url : resolvedUrls)
	{
		if (!resourceExists(url))
		{
			pageRaw->allResourcesOnPage.push_back(PageRawResource{ ResourceType::ResourceFlash, OnPageUrl{ url, UrlParameter::UnknownParameter } });
		}
	}
}

bool HtmlPageResourcesParser::resourceExists(const QUrl& resourceUrl) const noexcept
{
	QString resourceUrlStr = resourceUrl.toDisplayString();
	
	if (m_resourcesCache.contains(resourceUrlStr))
	{
		return true;
	}
	
	m_resourcesCache.insert(resourceUrlStr);
	return false;
}

}