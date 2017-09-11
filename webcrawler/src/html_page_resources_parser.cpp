#include "html_page_resources_parser.h"
#include "gumbo_parsing_helpers.h"
#include "page_raw_parser_helpers.h"

namespace WebCrawler
{

void HtmlPageResourcesParser::parse(GumboOutput* output, PageRawPtr& pageRaw) noexcept
{
	parseResourceType(output, pageRaw);
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
	if (pageRaw->contentType.startsWith("image/"))
	{
		pageRaw->resourceType = PageRawResource::ResourceImage;
		return;
	}

	if (pageRaw->contentType.startsWith("text/html") || 
		pageRaw->contentType.startsWith("text/xhtml") ||
		pageRaw->contentType.startsWith("application/xhtml") ||
		pageRaw->contentType.isEmpty())
	{
		pageRaw->resourceType = PageRawResource::ResourceHtml;
		return;
	}

	pageRaw->resourceType = PageRawResource::ResourceOther;
}

void HtmlPageResourcesParser::parseHtmlResources(GumboOutput* output, PageRawPtr& pageRaw) noexcept
{
	std::vector<QUrl> urls = GumboParsingHelpers::parsePageUrlList(output->root);

	std::vector<QUrl> resolvedUrls = PageRawParserHelpers::resolveUrlList(pageRaw->url, urls);

	for (const QUrl& url : resolvedUrls)
	{
		pageRaw->rawResources.push_back(PageRawResource{ PageRawResource::ResourceHtml, url, pageRaw->url });
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
		pageRaw->rawResources.push_back(PageRawResource{ PageRawResource::ResourceJavaScript, url, pageRaw->url });
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
		pageRaw->rawResources.push_back(PageRawResource{ PageRawResource::ResourceStyleSheet, url, pageRaw->url });
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
		pageRaw->rawResources.push_back(PageRawResource{ PageRawResource::ResourceImage, url, pageRaw->url });
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
		pageRaw->rawResources.push_back(PageRawResource{ PageRawResource::ResourceVideo, url, pageRaw->url });
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
		pageRaw->rawResources.push_back(PageRawResource{ PageRawResource::ResourceFlash, url, pageRaw->url });
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
		pageRaw->rawResources.push_back(PageRawResource{ PageRawResource::ResourceFlash, url, pageRaw->url });
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
		pageRaw->rawResources.push_back(PageRawResource{ PageRawResource::ResourceFlash, url, pageRaw->url });
	}
}

}