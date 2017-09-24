#include "crawler_worker_thread.h"
#include "crawler_url_storage.h"
#include "constants.h"
#include "gumbo_parsing_helpers.h"
#include "page_parser_helpers.h"

namespace WebCrawler
{

CrawlerWorkerThread::CrawlerWorkerThread(CrawlerUrlStorage* crawlerStorage, QueuedDownloader* queuedDownloader)
	: AbstractThreadableObject(this, QByteArray("CrawlerWorkerThread"))
	, m_pageParsedDataCollector(new PageParsedDataCollector(this))
	, m_crawlerInternalUrlStorage(crawlerStorage)
	, m_queuedDownloader(queuedDownloader)
{
	moveThisToSeparateThread();
}

void CrawlerWorkerThread::applyOptions(const CrawlerOptions& options)
{
	m_pageParsedDataCollector->setOptions(options);
}

void CrawlerWorkerThread::process()
{
	CrawlerRequest url;
	QueuedDownloader::Reply reply;

	const bool replyExtracted = m_queuedDownloader->extractReply(reply);
	const bool urlExtracted = m_crawlerInternalUrlStorage->extractUrl(url);

	if (urlExtracted)
	{
		m_queuedDownloader->scheduleUrl(url);
	}

	if (replyExtracted)
	{
		ParsedPagePtr page = m_pageParsedDataCollector->collectPageDataFromReply(reply);

		schedulePageResourcesLoading(page);

		emit pageParsed(page);
	}
}

void CrawlerWorkerThread::schedulePageResourcesLoading(const ParsedPagePtr& parsedPage)
{
	if (parsedPage->isThisExternalPage)
	{
		return;
	}

	std::vector<QUrl> urlList = m_pageParsedDataCollector->urlList();
	urlList = PageParserHelpers::resolveUrlList(parsedPage->url, urlList);
	m_crawlerInternalUrlStorage->saveUrlList(urlList, RequestTypeGet);

	if (!parsedPage->redirectedUrl.isEmpty())
	{
		m_crawlerInternalUrlStorage->saveUrlList(std::vector<QUrl>{ parsedPage->redirectedUrl }, RequestTypeGet);
	}

	std::vector<QUrl> resourcesUrlList;

	for (const RawResourceOnPage& resource : parsedPage->allResourcesOnPage)
	{
		const QString resourceUrlStr = resource.thisResourceUrl.url.toDisplayString();

		if (PageParserHelpers::isHttpOrHttpsScheme(resourceUrlStr) &&
			resource.resourceType != ResourceType::ResourceHtml)
		{
			resourcesUrlList.push_back(resource.thisResourceUrl.url);
		}
	}

	m_crawlerInternalUrlStorage->saveUrlList(resourcesUrlList, RequestTypeHead);
}

}