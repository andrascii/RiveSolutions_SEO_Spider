#include "page_parsed_data_collector.h"
#include "crawler_options.h"
#include "page_parser_helpers.h"
#include "html_page_title_parser.h"
#include "html_page_meta_parser.h"
#include "html_page_h_parser.h"
#include "html_page_word_count_parser.h"
#include "html_page_resources_parser.h"
#include "gumbo_parsing_helpers.h"

namespace WebCrawler
{

PageParsedDataCollector::PageParsedDataCollector(QObject* parent)
	: QObject(parent)
{
}

void PageParsedDataCollector::setOptions(const CrawlerOptions& options) noexcept
{
	m_options = options;

	applyOptions();
}

const CrawlerOptions& PageParsedDataCollector::options() const noexcept
{
	return m_options;
}

ParsedPagePtr PageParsedDataCollector::collectPageDataFromReply(const QueuedDownloader::Reply& reply)
{
	ParsedPagePtr page(new ParsedPage);

	collectReplyData(reply, page);

	if (page->pageSizeKilobytes > 512)
	{
		WARNINGLOG << "Page size is greater than 512 KB,"
			<< "size:" << page->pageSizeKilobytes << "KB,"
			<< "URL:" << reply.url.toString() << ","
			<< "Content-Type:" << page->contentType;
	}

	page->redirectedUrl = resolveRedirectUrl(reply);

	const QByteArray decodedHtmlPage = GumboParsingHelpers::decodeHtmlPage(reply.responseBody);
	GumboOutputCreatorDestroyerGuard gumboOutput(&kGumboDefaultOptions, decodedHtmlPage);

	collectParsedPageData(gumboOutput.output(), page);
	collectUrlList(gumboOutput.output());

#ifdef QT_DEBUG
	if (page->resourceType == ResourceType::ResourceHtml)
	{
		page->rawHtml = qCompress(decodedHtmlPage, 9);
	}
#endif

	return page;
}

const std::vector<QUrl>& PageParsedDataCollector::urlList() const noexcept
{
	return m_urlList;
}

void PageParsedDataCollector::applyOptions()
{
	m_parser.clear();

	m_parser.addParser(std::make_shared<HtmlPageResourcesParser>());
	m_parser.addParser(std::make_shared<HtmlPageMetaParser>());
	m_parser.addParser(std::make_shared<HtmlPageTitleParser>());
	m_parser.addParser(std::make_shared<HtmlPageHParser>());
	m_parser.addParser(std::make_shared<HtmlPageWordCountParser>());
}

QUrl PageParsedDataCollector::resolveRedirectUrl(const QueuedDownloader::Reply& reply) const
{
	if (reply.redirectUrl.isEmpty() || !reply.redirectUrl.isRelative())
	{
		return reply.redirectUrl;
	}

	return PageParserHelpers::resolveRelativeUrl(reply.redirectUrl, reply.url);
}

void PageParsedDataCollector::collectReplyData(const QueuedDownloader::Reply& reply, ParsedPagePtr& page)
{
	page->url = reply.url;
	page->statusCode = reply.statusCode;
	page->pageSizeKilobytes = reply.responseBody.size() / 1024;
	page->serverResponse = reply.responseHeaderValuePairs;
	page->pageHash = std::hash<std::string>()(reply.responseBody.toStdString().c_str());
	page->hasSeveralH1Tags = false;
	page->hasSeveralH2Tags = false;
	page->hasSeveralMetaDescriptionTags = false;
	page->hasSeveralMetaKeywordsTags = false;
	page->hasSeveralTitleTags = false;
	page->isThisExternalPage = PageParserHelpers::isUrlExternal(options().host, page->url);

	page->contentType = QString();

	foreach(QByteArray key, reply.responseHeaders.keys())
	{
		if (key.toLower() != "content-type")
		{
			continue;
		}

		page->contentType = reply.responseHeaders.value(key);
		break;
	}
}

void PageParsedDataCollector::collectParsedPageData(GumboOutput* output, ParsedPagePtr& page)
{
	m_parser.parse(output, page);
}

void PageParsedDataCollector::collectUrlList(GumboOutput* output)
{
	m_urlList.clear();

	std::vector<LinkInfo> linksOnPage = GumboParsingHelpers::parsePageUrlList(output->root, true);

	for (const LinkInfo& linkInfo : linksOnPage)
	{
		m_urlList.push_back(linkInfo.url);
	}
}

}