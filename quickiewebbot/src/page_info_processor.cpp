#include "page_info_processor.h"
#include "service_locator.h"
#include "web_crawler_internal_url_storage.h"
#include "constants.h"
#include "html_page_title_parser.h"
#include "html_page_meta_parser.h"

namespace QuickieWebBot
{

PageInfoProcessor::PageInfoProcessor(WebCrawlerInternalUrlStorage* crawlerStorage, QueuedDownloader* queuedDownloader, QObject* parent)
	: AbstractThreadableObject(this, QByteArray("QuickieWebBot::PageInfoProcessorThread"))
	, m_webCrawlerInternalUrlStorage(crawlerStorage)
	, m_queuedDownloader(queuedDownloader)
	, m_pageInfo(new PageInfo)
{
	moveThisToSeparateThread();

	m_htmlPageParser.addPageInfoParser(std::make_shared<HtmlPageMetaParser>());
	m_htmlPageParser.addPageInfoParser(std::make_shared<HtmlPageTitleParser>());
}

void PageInfoProcessor::process()
{
	QUrl url;
	QueuedDownloader::Reply reply;

	const bool replyExtracted = m_queuedDownloader->extractReply(reply);
	const bool urlExtracted = m_webCrawlerInternalUrlStorage->extractUrl(url);

	if (urlExtracted)
	{
		m_queuedDownloader->scheduleUrl(url);
	}

	if (replyExtracted)
	{
		m_pageInfo.reset(new PageInfo);

		m_htmlPageParser.parsePage(reply.responseBody, m_pageInfo);

		m_pageInfo->url = reply.url;
		m_pageInfo->serverResponse = reply.responseHeaderValuePairs;

		std::vector<QUrl> urlList = m_htmlPageParser.pageUrlList();

		validateUrlList(urlList);
		m_webCrawlerInternalUrlStorage->saveUrlList(urlList);

		emit webPageParsed(m_pageInfo);
	}
}

void PageInfoProcessor::validateUrlList(std::vector<QUrl>& urlList) noexcept
{
	for (QUrl& url : urlList)
	{
		
	}
}

}