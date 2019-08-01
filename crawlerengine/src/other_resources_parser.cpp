#include "stdafx.h"
#include "other_resources_parser.h"
#include "page_parser_helpers.h"
#include "ihtml_parser.h"

namespace CrawlerEngine
{
	
OtherResourcesParser::OtherResourcesParser(IHtmlParser* htmlParser)
	: m_htmlParser(htmlParser)
{
}

void OtherResourcesParser::parse(const ResponseHeaders& headers, ParsedPagePtr& page)
{
	Q_UNUSED(headers);

	if (page->resourceType != ResourceType::ResourceHtml)
	{
		return;
	}

	std::vector<LinkInfo> linksInfo = m_htmlParser->pageUrlList(false);

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
