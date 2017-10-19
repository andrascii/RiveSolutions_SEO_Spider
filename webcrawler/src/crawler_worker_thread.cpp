#include "crawler_worker_thread.h"
#include "crawler_url_storage.h"
#include "constants.h"
#include "gumbo_parsing_helpers.h"
#include "page_parser_helpers.h"
#include "iqueued_dowloader.h"

namespace WebCrawler
{

CrawlerWorkerThread::CrawlerWorkerThread(CrawlerUrlStorage* crawlerStorage, IQueuedDownloader* queuedDownloader)
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
	IQueuedDownloader::Reply reply;

	const bool replyExtracted = m_queuedDownloader->extractReply(reply);
	const bool urlExtracted = m_crawlerInternalUrlStorage->extractUrl(url);

	if (urlExtracted)
	{
		m_queuedDownloader->scheduleUrl(url);
	}

	if (replyExtracted)
	{
		const ParsedPagePtr page = m_pageParsedDataCollector->collectPageDataFromReply(reply);

		schedulePageResourcesLoading(page);

		emit pageParsed(page);
	}
}

void CrawlerWorkerThread::schedulePageResourcesLoading(const ParsedPagePtr& parsedPage) const
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

	std::vector<QUrl> resourcesHeadUrlList;
	std::vector<QUrl> resourcesGetUrlList;

	for (const RawResourceOnPage& resource : parsedPage->allResourcesOnPage)
	{
		const QString resourceUrlStr = resource.thisResourceLink.url.toDisplayString();

		if (PageParserHelpers::isHttpOrHttpsScheme(resourceUrlStr) &&
			resource.resourceType != ResourceType::ResourceHtml)
		{
			if (resource.resourceType == ResourceType::ResourceImage)
			{
				resourcesGetUrlList.push_back(resource.thisResourceLink.url);
			}
			else
			{
				resourcesHeadUrlList.push_back(resource.thisResourceLink.url);
			}
		}
	}

	m_crawlerInternalUrlStorage->saveUrlList(resourcesGetUrlList, RequestTypeGet);
	m_crawlerInternalUrlStorage->saveUrlList(resourcesHeadUrlList, RequestTypeHead);
}

}