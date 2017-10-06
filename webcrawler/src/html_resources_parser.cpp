#include "html_resources_parser.h"
#include "meta_parser.h"
#include "title_parser.h"
#include "h_parser.h"
#include "word_count_parser.h"
#include "gumbo_parsing_helpers.h"
#include "page_parser_helpers.h"
#include "resources_cache.h"

namespace WebCrawler
{

HtmlResourcesParser::HtmlResourcesParser(ResourcesCache* resourcesCache)
	: m_resourcesCache(resourcesCache)
{
	addParser(std::make_shared<MetaParser>());
	addParser(std::make_shared<TitleParser>());
	addParser(std::make_shared<HParser>());
	addParser(std::make_shared<WordCountParser>());
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

		if (m_resourcesCache->isResourceExists(linkInfo.url))
		{
			continue;
		}

		ResourceType resourceType = PageParserHelpers::isHttpOrHttpsScheme(linkInfo.url.toDisplayString()) ?
			ResourceType::ResourceHtml : ResourceType::ResourceOther;

		page->allResourcesOnPage.push_back(RawResourceOnPage{ resourceType, linkInfo, ResourceSource::SourceTagA });
	}

	CompoundParser::parse(output, page);
}

void HtmlResourcesParser::init()
{
	m_resourcesCache->clear();
}

}