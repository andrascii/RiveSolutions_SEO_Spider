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
}

void HtmlPageResourcesParser::parseResourceType(GumboOutput* output, PageRawPtr& pageRaw) noexcept
{
	if (pageRaw->contentType.startsWith("text/html") || pageRaw->contentType.isEmpty())
	{
		pageRaw->resourceType = PageRawResource::ResourceHtml;
	}
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
			gumbo_get_attribute(&node->v.element.attributes, "rel") &&
			QString(gumbo_get_attribute(&node->v.element.attributes, "rel")->value).toLower() == "stylesheet";
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
		bool result = node &&
			node->type == GUMBO_NODE_ELEMENT &&
			node->v.element.tag == GUMBO_TAG_VIDEO &&
			gumbo_get_attribute(&node->v.element.attributes, "src");

		if (!result)
		{
			return result;
		}

		const GumboVector* children = &node->v.element.children;
		if (children->length == 0)
		{
			return false;
		}

		const GumboNode* child = static_cast<const GumboNode*>(children->data[0]);
		return child &&
			child->type == GUMBO_NODE_ELEMENT &&
			child->v.element.tag == GUMBO_TAG_SOURCE &&
			gumbo_get_attribute(&node->v.element.attributes, "src");

	};

	auto res = [](const GumboNode* node)
	{
		const GumboVector* children = &node->v.element.children;
		const GumboNode* child = static_cast<const GumboNode*>(children->data[0]);
		GumboAttribute* src = gumbo_get_attribute(&node->v.element.attributes, "src");
		return QUrl(src->value);
	};

	std::vector<QUrl> urls = GumboParsingHelpers::findNodesAndGetResult(output->root, cond, res);
	std::vector<QUrl> resolvedUrls = PageRawParserHelpers::resolveUrlList(pageRaw->url, urls);

	for (const QUrl& url : resolvedUrls)
	{
		pageRaw->rawResources.push_back(PageRawResource{ PageRawResource::ResourceVideo, url, pageRaw->url });
	}
}

}