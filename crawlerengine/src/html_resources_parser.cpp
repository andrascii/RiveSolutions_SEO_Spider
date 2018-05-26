#include "html_resources_parser.h"
#include "meta_parser.h"
#include "title_parser.h"
#include "h_parser.h"
#include "word_count_parser.h"
#include "page_parser_helpers.h"
#include "data_resources_parser.h"
#include "ihtml_parser.h"
#include "frames_detector_parser.h"
#include "myhtml_parser.h"

namespace CrawlerEngine
{

HtmlResourcesParser::HtmlResourcesParser(IHtmlParser* htmlParser)
	: m_htmlParser(htmlParser)
{
	m_htmlParserTest = new MyHtmlParser;

	addParser(std::make_shared<MetaParser>(m_htmlParser));
	addParser(std::make_shared<FramesDetectorParser>(m_htmlParser));
	addParser(std::make_shared<TitleParser>(m_htmlParser));
	addParser(std::make_shared<HParser>(m_htmlParser));
	addParser(std::make_shared<WordCountParser>(m_htmlParser));
	addParser(std::make_shared<DataResourcesParser>(ResourceType::ResourceHtml));
}

void HtmlResourcesParser::parse(const ResponseHeaders& headers, ParsedPagePtr& page)
{
	if (page->resourceType != ResourceType::ResourceHtml)
	{
		CompoundParser::parse(headers, page);
		return;
	}

	std::vector<LinkInfo> linksInfo = m_htmlParser->pageUrlList(true);

	DEBUG_ASSERT(page->baseUrl.isValid());

	for (LinkInfo& linkInfo : linksInfo)
	{
		linkInfo.url = PageParserHelpers::resolveUrl(page->baseUrl, linkInfo.url);

		if (!PageParserHelpers::isHttpOrHttpsScheme(linkInfo.url))
		{
			DEBUG_ASSERT(!"This url leads to a not html resources");
			continue;
		}

		if (linkInfo.resourceSource == ResourceSource::SourceTagLinkRelCanonical)
		{
			page->canonicalUrl = linkInfo.url;
		}

		ResourceOnPage resource{ ResourceType::ResourceHtml, std::move(linkInfo) };

		if (page->allResourcesOnPage.find(resource) == page->allResourcesOnPage.end())
		{
			page->allResourcesOnPage.insert(std::move(resource));
		}
	}

	CompoundParser::parse(headers, page);
}

}
