#include "page_info_processor.h"
#include "service_locator.h"
#include "web_crawler_internal_url_storage.h"
#include "constants.h"

namespace QuickieWebBot
{

PageInfoProcessor::PageInfoProcessor(WebCrawlerInternalUrlStorage* crawlerStorage, QueuedDownloader* queuedDownloader, QObject* parent)
	: AbstractThreadableObject(this)
	, m_webCrawlerInternalUrlStorage(crawlerStorage)
	, m_queuedDownloader(queuedDownloader)
	, m_pageInfo(new PageInfo)
{
	moveThisToSeparateThread<PageInfoProcessor>();
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

		m_webCrawlerInternalUrlStorage->saveUrlList(m_htmlPageParser.pageUrlList());

		emit webPageParsed(m_pageInfo);
	}
}

}