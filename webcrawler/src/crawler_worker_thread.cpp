#include "crawler_worker_thread.h"
#include "unique_link_store.h"
#include "page_parser_helpers.h"
#include "iqueued_dowloader.h"

namespace WebCrawler
{

CrawlerWorkerThread::CrawlerWorkerThread(UniqueLinkStore* crawlerStorage, IQueuedDownloader* queuedDownloader)
	: AbstractThreadableObject(this, QByteArray("CrawlerWorkerThread"))
	, m_pageParsedDataCollector(new PageParsedDataCollector(this))
	, m_uniqueLinkStore(crawlerStorage)
	, m_queuedDownloader(queuedDownloader)
{
	moveThisToSeparateThread();
}

void CrawlerWorkerThread::applyOptions(const CrawlerOptions& options, RobotsTxtRules robotsTxtRules)
{
	m_optionsLinkFilter.reset(new OptionsLinkFilter(options, robotsTxtRules));
	
	m_pageParsedDataCollector->setOptions(options);
}

void CrawlerWorkerThread::process()
{
	CrawlerRequest url;
	IQueuedDownloader::Reply reply;

	const bool replyExtracted = m_queuedDownloader->extractReply(reply);
	const bool urlExtracted = m_uniqueLinkStore->extractUrl(url);

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
	m_uniqueLinkStore->saveUrlList(urlList, RequestTypeGet);

	if (!parsedPage->redirectedUrl.isEmpty())
	{
		m_uniqueLinkStore->saveUrlList(std::vector<QUrl>{ parsedPage->redirectedUrl }, RequestTypeGet);
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

	m_uniqueLinkStore->saveUrlList(resourcesGetUrlList, RequestTypeGet);
	m_uniqueLinkStore->saveUrlList(resourcesHeadUrlList, RequestTypeHead);
}

}