#include "html_resources_parser.h"
#include "meta_parser.h"
#include "title_parser.h"
#include "h_parser.h"
#include "word_count_parser.h"
#include "gumbo_parsing_helpers.h"
#include "page_parser_helpers.h"
#include "resources_cache.h"
#include "data_resources_parser.h"

namespace WebCrawler
{

HtmlResourcesParser::HtmlResourcesParser()
{
	addParser(std::make_shared<MetaParser>());
	addParser(std::make_shared<TitleParser>());
	addParser(std::make_shared<HParser>());
	addParser(std::make_shared<WordCountParser>());
	addParser(std::make_shared<DataResourcesParser>(ResourceType::ResourceHtml));
}

void HtmlResourcesParser::parse(GumboOutput* output, ParsedPagePtr& page)
{
	if (page->resourceType != ResourceType::ResourceHtml)
	{
		return;
	}

	std::vector<LinkInfo> linksInfo = GumboParsingHelpers::parsePageUrlList(output->root, false);

	for (LinkInfo& linkInfo : linksInfo)
	{
		linkInfo.url = PageParserHelpers::resolveUrl(page->url, linkInfo.url);

		const ResourceType resourceType = PageParserHelpers::isHttpOrHttpsScheme(linkInfo.url.toDisplayString()) ?
			ResourceType::ResourceHtml : ResourceType::ResourceOther;

		page->allResourcesOnPage.insert(RawResourceOnPage{ resourceType, linkInfo, ResourceSource::SourceTagA });
	}

	CompoundParser::parse(output, page);
}

}