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
	std::vector<QUrl> unresolvedUrls;

	for (const LinkInfo& linkInfo : linksInfo)
	{
		unresolvedUrls.push_back(linkInfo.url);
	}

	std::vector<QUrl> resolvedUrls = PageParserHelpers::resolveUrlList(page->url, unresolvedUrls);

	for (size_t i = 0; i < resolvedUrls.size(); ++i)
	{
		const QUrl& url = resolvedUrls[i];

		if (m_resourcesCache->isResourceExists(url))
		{
			continue;
		}

		ResourceType resourceType = PageParserHelpers::isHttpOrHttpsScheme(url.toDisplayString()) ?
			ResourceType::ResourceHtml : ResourceType::ResourceOther;

		page->allResourcesOnPage.push_back(RawResourceOnPage{ resourceType, LinkInfo{ url, linksInfo[i].urlParameter } });
	}

	CompoundParser::parse(output, page);
}

void HtmlResourcesParser::init()
{
	m_resourcesCache->clear();
}

}