#include "crawler_worker_thread.h"
#include "unique_link_store.h"
#include "page_parser_helpers.h"
#include "page_data_collector.h"
#include "options_link_filter.h"
#include "download_request.h"
#include "download_response.h"
#include "crawler.h"

namespace CrawlerEngine
{

CrawlerWorkerThread::CrawlerWorkerThread(UniqueLinkStore* uniqueLinkStore)
	: QObject(nullptr)
	, m_pageDataCollector(new PageDataCollector(this))
	, m_uniqueLinkStore(uniqueLinkStore)
	, m_isRunning(false)
{
	VERIFY(connect(m_uniqueLinkStore, &UniqueLinkStore::urlAdded, this,
		&CrawlerWorkerThread::extractUrlAndDownload, Qt::QueuedConnection));

	VERIFY(connect(&Crawler::instance(), &Crawler::onAboutClearData, 
		this, &CrawlerWorkerThread::onCrawlerClearData, Qt::QueuedConnection));
}

void CrawlerWorkerThread::startWithOptions(const CrawlerOptions& options, RobotsTxtRules robotsTxtRules)
{
	ASSERT(thread() == QThread::currentThread());

	m_isRunning = true;
	m_optionsLinkFilter.reset(new OptionsLinkFilter(options, robotsTxtRules));
	m_pageDataCollector->setOptions(options);

	onStart();
	extractUrlAndDownload();
}

void CrawlerWorkerThread::stop()
{
	ASSERT(thread() == QThread::currentThread());

	m_isRunning = false;
}

void CrawlerWorkerThread::extractUrlAndDownload()
{
	if (!m_isRunning)
	{
		return;
	}

	if (m_downloadRequester)
	{
		return;
	}

	CrawlerRequest url;
	const bool isUrlExtracted = m_uniqueLinkStore->extractUrl(url);

	if (isUrlExtracted)
	{
		DownloadRequest request(url);
		m_downloadRequester.reset(request, this, &CrawlerWorkerThread::onLoadingDone);
		m_downloadRequester->start();
	}
}

void CrawlerWorkerThread::onCrawlerClearData()
{
	m_pagesAcceptedAfterStop.clear();
}

void CrawlerWorkerThread::schedulePageResourcesLoading(const ParsedPagePtr& parsedPage) const
{
	if (parsedPage->isThisExternalPage)
	{
		return;
	}

	std::vector<LinkInfo> outlinks = m_pageDataCollector->outlinks();
	outlinks = PageParserHelpers::resolveUrlList(parsedPage->url, outlinks);

	handlePageLinkList(outlinks, parsedPage->metaRobotsFlags);

	m_uniqueLinkStore->saveLinkList(outlinks, DownloadRequestType::RequestTypeGet);

	if (!parsedPage->redirectedUrl.isEmpty())
	{
		m_uniqueLinkStore->saveUrlList(std::vector<QUrl>{ parsedPage->redirectedUrl }, DownloadRequestType::RequestTypeGet);
	}

	std::vector<QUrl> resourcesHeadUrlList;
	std::vector<QUrl> resourcesGetUrlList;

	for (const RawResourceOnPage& resource : parsedPage->allResourcesOnPage)
	{
		if (PageParserHelpers::isHttpOrHttpsScheme(resource.thisResourceLink.url) &&
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

	m_uniqueLinkStore->saveUrlList(resourcesGetUrlList, DownloadRequestType::RequestTypeGet);

	m_uniqueLinkStore->saveUrlList(resourcesHeadUrlList, DownloadRequestType::RequestTypeHead);
}

void CrawlerWorkerThread::handlePageLinkList(std::vector<LinkInfo>& linkList, const MetaRobotsFlagsSet& metaRobotsFlags) const
{
	const auto isNofollowLinkUnavailable = [optionsLinkFilter = m_optionsLinkFilter.get(), metaRobotsFlags](const LinkInfo& linkInfo)
	{
		return optionsLinkFilter->linkPermission(linkInfo, metaRobotsFlags) == OptionsLinkFilter::PermissionNofollowNotAllowed;
	};

	const auto isLinkBlockedByRobotsTxt = [optionsLinkFilter = m_optionsLinkFilter.get(), metaRobotsFlags](const LinkInfo& linkInfo)
	{
		return optionsLinkFilter->linkPermission(linkInfo, metaRobotsFlags) == OptionsLinkFilter::PermissionBlockedByRobotsTxtRules;
	};

	const auto isSubdomainLinkUnavailable = [optionsLinkFilter = m_optionsLinkFilter.get(), metaRobotsFlags](const LinkInfo& linkInfo)
	{
		return optionsLinkFilter->linkPermission(linkInfo, metaRobotsFlags) == OptionsLinkFilter::PermissionSubdomainNotAllowed;
	};

	const auto removeUrlLastSlashIfExists = [](LinkInfo& link)
	{
		PageParserHelpers::removeUrlLastSlashIfExists(link.url);
	};

	linkList.erase(std::remove_if(linkList.begin(), linkList.end(), isNofollowLinkUnavailable), linkList.end());

	linkList.erase(std::remove_if(linkList.begin(), linkList.end(), isSubdomainLinkUnavailable), linkList.end());

	const auto emitPageParsedForBlockedPages = [this](const LinkInfo& linkInfo)
	{
		ParsedPagePtr page(new ParsedPage);

		page->url = linkInfo.url;
		page->title = tr("Blocked by robots.txt rules");

		//
		// TODO: change it on ResourceType::ResourceUnknown
		// It adding new type requires rework of ModelController
		//
		page->resourceType = ResourceType::ResourceHtml;

		emit pageParsed(page);
	};

	const auto blockedByRobotsTxtLinksIterator = std::remove_if(linkList.begin(), linkList.end(), isLinkBlockedByRobotsTxt);

	std::for_each(blockedByRobotsTxtLinksIterator, linkList.end(), emitPageParsedForBlockedPages);

	linkList.erase(blockedByRobotsTxtLinksIterator, linkList.end());

	std::for_each(linkList.begin(), linkList.end(), removeUrlLastSlashIfExists);
}

void CrawlerWorkerThread::onLoadingDone(Requester* requester, const DownloadResponse& response)
{
	m_downloadRequester.reset();

	const ParsedPagePtr page = m_pageDataCollector->collectPageDataFromResponse(response);

	schedulePageResourcesLoading(page);

	if (!m_isRunning)
	{
		m_pagesAcceptedAfterStop.push_back(page);
		return;
	}
 
	emit pageParsed(page);

	extractUrlAndDownload();
}

void CrawlerWorkerThread::onStart()
{
	if (!m_pagesAcceptedAfterStop.empty())
	{
		for (const ParsedPagePtr& page : m_pagesAcceptedAfterStop)
		{
			emit pageParsed(page);
		}
	}

	m_pagesAcceptedAfterStop.clear();
}

}