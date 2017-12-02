#include "html_resources_parser.h"
#include "meta_parser.h"
#include "title_parser.h"
#include "h_parser.h"
#include "word_count_parser.h"
#include "page_parser_helpers.h"
#include "data_resources_parser.h"
#include "gumbo_parsing_helpers.h"

namespace CrawlerEngine
{

HtmlResourcesParser::HtmlResourcesParser()
{
	addParser(std::make_shared<MetaParser>());
	addParser(std::make_shared<TitleParser>());
	addParser(std::make_shared<HParser>());
	addParser(std::make_shared<WordCountParser>());
	addParser(std::make_shared<DataResourcesParser>(ResourceType::ResourceHtml));
}

void HtmlResourcesParser::parse(GumboOutput* output, const ResponseHeaders& headers, ParsedPagePtr& page)
{
	if (page->resourceType != ResourceType::ResourceHtml)
	{
		CompoundParser::parse(output, headers, page);
		return;
	}

	std::vector<LinkInfo> linksInfo = GumboParsingHelpers::parsePageUrlList(output->root, true);

	for (LinkInfo& linkInfo : linksInfo)
	{
		linkInfo.url = PageParserHelpers::resolveUrl(page->url, linkInfo.url);

		if (!PageParserHelpers::isHttpOrHttpsScheme(linkInfo.url))
		{
			DEBUG_ASSERT(false);
			continue;
		}

		if (linkInfo.resourceSource == ResourceSource::SourceTagLinkRelCanonical)
		{
			page->canonicalUrl = linkInfo.url;
		}

		page->allResourcesOnPage.emplace_back(RawResourceOnPage{ ResourceType::ResourceHtml, std::move(linkInfo) });
	}

	CompoundParser::parse(output, headers, page);
}

}
