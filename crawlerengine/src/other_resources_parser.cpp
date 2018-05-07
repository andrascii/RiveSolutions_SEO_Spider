#include "other_resources_parser.h"

#include "page_parser_helpers.h"
#include "gumbo_parsing_helpers.h"

namespace CrawlerEngine
{
	
void OtherResourcesParser::parse(GumboOutput* output, const ResponseHeaders& headers, ParsedPagePtr& page)
{
	Q_UNUSED(headers);

	if (page->resourceType != ResourceType::ResourceHtml)
	{
		return;
	}

	std::vector<LinkInfo> linksInfo = GumboParsingHelpers::parsePageUrlList(output->root, false);

	for (LinkInfo& linkInfo : linksInfo)
	{
		linkInfo.url = PageParserHelpers::resolveUrl(page->baseUrl, linkInfo.url);

		if (PageParserHelpers::isHttpOrHttpsScheme(linkInfo.url))
		{
			continue;
		}

		ResourceOnPage resource{ ResourceType::ResourceOther, std::move(linkInfo) };

		if (page->allResourcesOnPage.find(resource) == page->allResourcesOnPage.end())
		{
			page->allResourcesOnPage.insert(std::move(resource));
		}
	}

}

}
