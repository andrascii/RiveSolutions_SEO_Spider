#include "html_page_resources_parser.h"
#include "gumbo_parsing_helpers.h"
#include "page_raw_parser_helpers.h"

namespace WebCrawler
{

void HtmlPageResourcesParser::parse(GumboOutput* output, PageRawPtr& pageRaw) noexcept
{
	parseResourceType(output, pageRaw);
	
}

void HtmlPageResourcesParser::parseResourceType(GumboOutput* output, PageRawPtr& pageRaw) noexcept
{
	if (pageRaw->contentType.startsWith("text/html") || pageRaw->contentType.isEmpty())
	{
		pageRaw->resourceType = PageRawResource::ResourceHtml;
	}

	parseHtmlResources(output, pageRaw);
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

}