#include "crawler_worker_thread.h"
#include "unique_link_store.h"
#include "page_parser_helpers.h"
#include "page_data_collector.h"
#include "options_link_filter.h"
#include "download_request.h"
#include "download_response.h"
#include "crawler.h"
#include "iuniqueness_checker.h"


namespace CrawlerEngine
{

CrawlerWorkerThread::CrawlerWorkerThread(UniqueLinkStore* uniqueLinkStore)
	: QObject(nullptr)
	, m_pageDataCollector(new PageDataCollector(this))
	, m_uniqueLinkStore(uniqueLinkStore)
	, m_isRunning(false)
	, m_defferedProcessingTimer(new QTimer(this))
	, m_uniquenessChecker(createUniquenessChecker())
{
	VERIFY(connect(m_uniqueLinkStore, &UniqueLinkStore::urlAdded, this,
		&CrawlerWorkerThread::extractUrlAndDownload, Qt::QueuedConnection));

	VERIFY(connect(&Crawler::instance(), &Crawler::onAboutClearData, 
		this, &CrawlerWorkerThread::onCrawlerClearData, Qt::QueuedConnection));

	VERIFY(connect(m_defferedProcessingTimer, &QTimer::timeout, 
		this, &CrawlerWorkerThread::extractUrlAndDownload));
	
	m_defferedProcessingTimer->setInterval(1000);
	m_defferedProcessingTimer->setSingleShot(true);
}

std::future<std::vector<CrawlerRequest>> CrawlerWorkerThread::pendingUrls() const
{
	return m_pagesAcceptedAfterStop.pagesAcceptedPromise.get_future();
}

void CrawlerWorkerThread::startWithOptions(const CrawlerOptions& options, RobotsTxtRules robotsTxtRules)
{
	DEBUG_ASSERT(thread() == QThread::currentThread());

	m_isRunning = true;
	m_optionsLinkFilter.reset(new OptionsLinkFilter(options, robotsTxtRules));
	m_pageDataCollector->setOptions(options);

	onStart();
	extractUrlAndDownload();
}

void CrawlerWorkerThread::stop()
{
	DEBUG_ASSERT(thread() == QThread::currentThread());

	m_isRunning = false;

	if (!m_downloadRequester)
	{
		try
		{
			DEBUGLOG << "Set value to promise";

			m_pagesAcceptedAfterStop.pagesAcceptedPromise.set_value(preparePagesAfterStop());
		}
		catch (const std::future_error& error)
		{
			if (error.code() != std::make_error_condition(std::future_errc::promise_already_satisfied))
			{
				throw;
			}
		}
	}
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

	const CrawlerSharedState* state = CrawlerSharedState::instance();
	const int workersProcessedLinksCount = state->workersProcessedLinksCount();
	const int modelControllerCrawledLinksCount = state->modelControllerCrawledLinksCount();

	const int pendingLinksCount = workersProcessedLinksCount - modelControllerCrawledLinksCount;
	constexpr int maxPendingLinksCount = 15;

	if (pendingLinksCount > maxPendingLinksCount)
	{
		m_defferedProcessingTimer->start();
		return;
	}

	CrawlerRequest crawlerRequest;
	const bool isUrlExtracted = m_uniqueLinkStore->extractUrl(crawlerRequest);

	if (isUrlExtracted)
	{
		ASSERT(!m_uniquenessChecker->hasRequest(crawlerRequest));
		m_uniquenessChecker->registrateRequest(crawlerRequest);

		DownloadRequest request(crawlerRequest);
		m_currentRequest = crawlerRequest;
		m_downloadRequester.reset(request, this, &CrawlerWorkerThread::onLoadingDone);
		m_downloadRequester->start();
	}
}

void CrawlerWorkerThread::onCrawlerClearData()
{
	m_pagesAcceptedAfterStop.pages.clear();
	m_currentRequest.reset();
	CrawlerSharedState::instance()->setWorkersProcessedLinksCount(0);
}

void CrawlerWorkerThread::schedulePageResourcesLoading(ParsedPagePtr& parsedPage)
{
	if (parsedPage->isThisExternalPage)
	{
		return;
	}

	std::vector<LinkInfo> outlinks = m_pageDataCollector->outlinks();
	outlinks = PageParserHelpers::resolveUrlList(parsedPage->url, outlinks);

	handlePageLinkList(outlinks, parsedPage->metaRobotsFlags, parsedPage);

	m_uniqueLinkStore->addLinkList(std::move(outlinks), DownloadRequestType::RequestTypeGet);

	if (!parsedPage->redirectedUrl.isEmpty())
	{
		m_uniqueLinkStore->addUrlList(std::vector<Url>{ parsedPage->redirectedUrl }, DownloadRequestType::RequestTypeGet);
	}

	std::vector<Url> resourcesHeadUrlList;
	std::vector<Url> resourcesGetUrlList;

	for (const ResourceOnPage& resource : parsedPage->allResourcesOnPage)
	{
		if (PageParserHelpers::isHttpOrHttpsScheme(resource.link.url) &&
			resource.resourceType != ResourceType::ResourceHtml)
		{
			if (resource.resourceType == ResourceType::ResourceImage)
			{
				resourcesGetUrlList.push_back(resource.link.url);
			}
			else
			{
				resourcesHeadUrlList.push_back(resource.link.url);
			}
		}
	}

	m_uniqueLinkStore->addUrlList(resourcesGetUrlList, DownloadRequestType::RequestTypeGet);
	m_uniqueLinkStore->addUrlList(resourcesHeadUrlList, DownloadRequestType::RequestTypeHead);
}

void CrawlerWorkerThread::handlePageLinkList(std::vector<LinkInfo>& linkList, const MetaRobotsFlagsSet& metaRobotsFlags, ParsedPagePtr& parsedPage)
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

	const auto isExternalLinkUnavailable = [optionsLinkFilter = m_optionsLinkFilter.get(), metaRobotsFlags](const LinkInfo& linkInfo)
	{
		return optionsLinkFilter->linkPermission(linkInfo, metaRobotsFlags) == OptionsLinkFilter::PermissionExternalLinksNotAllowed;
	};

	const auto isOutsideFolderLinkUnavailable = [optionsLinkFilter = m_optionsLinkFilter.get(), metaRobotsFlags](const LinkInfo& linkInfo)
	{
		return optionsLinkFilter->linkPermission(linkInfo, metaRobotsFlags) == OptionsLinkFilter::PermissionBlockedByFolder;
	};

	const auto setLinkLoadAvailability = [&](ResourceOnPage& resource)
	{
		const bool loadAvailability = PageParserHelpers::isHttpOrHttpsScheme(resource.link.url) &&
			!isNofollowLinkUnavailable(resource.link) &&
			!isSubdomainLinkUnavailable(resource.link) &&
			!isLinkBlockedByRobotsTxt(resource.link) &&
			!isExternalLinkUnavailable(resource.link) &&
			!isOutsideFolderLinkUnavailable(resource.link);

		resource.loadAvailability = loadAvailability;
	};

	linkList.erase(std::remove_if(linkList.begin(), linkList.end(), isNofollowLinkUnavailable), linkList.end());
	linkList.erase(std::remove_if(linkList.begin(), linkList.end(), isSubdomainLinkUnavailable), linkList.end());
	linkList.erase(std::remove_if(linkList.begin(), linkList.end(), isExternalLinkUnavailable), linkList.end());
	linkList.erase(std::remove_if(linkList.begin(), linkList.end(), isOutsideFolderLinkUnavailable), linkList.end());

	ResourcesOnPageList resources;
	for (const ResourceOnPage& resource : parsedPage->allResourcesOnPage)
	{
		ResourceOnPage fixedResource = resource;
		setLinkLoadAvailability(fixedResource);
		resources.insert(fixedResource);
	}
	
	parsedPage->allResourcesOnPage = resources;

	const auto emitPageParsedForBlockedPages = [this](const LinkInfo& linkInfo)
	{
		if (m_uniqueLinkStore->addCrawledUrl(linkInfo.url, DownloadRequestType::RequestTypeGet))
		{
			ParsedPagePtr page(new ParsedPage);

			page->url = linkInfo.url;
			page->statusCode = Common::StatusCode::BlockedByRobotsTxt;
			page->resourceType = ResourceType::ResourceHtml;

			onPageParsed(page);
		}
	};

	const auto blockedByRobotsTxtLinksIterator = std::remove_if(linkList.begin(), linkList.end(), isLinkBlockedByRobotsTxt);
	std::for_each(blockedByRobotsTxtLinksIterator, linkList.end(), emitPageParsedForBlockedPages);
	linkList.erase(blockedByRobotsTxtLinksIterator, linkList.end());
}

void CrawlerWorkerThread::onLoadingDone(Requester*, const DownloadResponse& response)
{
	m_downloadRequester.reset();

	std::vector<ParsedPagePtr> pages = m_pageDataCollector->collectPageDataFromResponse(response);

	ASSERT(m_currentRequest.has_value());
	const DownloadRequestType requestType = m_currentRequest.value().requestType;

	bool checkUrl = false;

	for (ParsedPagePtr& page : pages)
	{
		schedulePageResourcesLoading(page);

		const bool urlAdded = !checkUrl || m_uniqueLinkStore->addCrawledUrl(page->url, requestType);

		if (urlAdded && !m_isRunning)
		{
			m_pagesAcceptedAfterStop.pages.push_back(std::make_pair(m_currentRequest.value().requestType, page));

			continue;
		}

		if (urlAdded)
		{
			onPageParsed(page);
		}

		checkUrl = true;
	}

	if (!m_isRunning)
	{
		DEBUGLOG << "Set value to promise";

		try
		{
			m_pagesAcceptedAfterStop.pagesAcceptedPromise.set_value(preparePagesAfterStop());
		}
		catch (const std::future_error& error)
		{
			if (error.code() != std::make_error_condition(std::future_errc::promise_already_satisfied))
			{
				DEBUG_ASSERT(!"At this stage promise must not be valid! Possibly problem in downloader!");
				throw;
			}
		}
	}

	extractUrlAndDownload();
}

void CrawlerWorkerThread::onStart()
{
	if (!m_pagesAcceptedAfterStop.pages.empty())
	{
		for (const PagesAcceptedAfterStop::PageRequestPair& pair : m_pagesAcceptedAfterStop.pages)
		{
			onPageParsed(pair.second);
		}
	}

	m_pagesAcceptedAfterStop.pages.clear();

	m_pagesAcceptedAfterStop.pagesAcceptedPromise = std::promise<std::vector<CrawlerRequest>>();
}

void CrawlerWorkerThread::onPageParsed(const ParsedPagePtr& parsedPage) const noexcept
{
	emit pageParsed(parsedPage);

	CrawlerSharedState::instance()->incrementWorkersProcessedLinksCount();
}

std::vector<CrawlerRequest> CrawlerWorkerThread::preparePagesAfterStop() const
{
	std::vector<CrawlerRequest> result;
	result.reserve(m_pagesAcceptedAfterStop.pages.size());

	for (const PagesAcceptedAfterStop::PageRequestPair& pair : m_pagesAcceptedAfterStop.pages)
	{
		result.emplace_back(CrawlerRequest{ pair.second->url, pair.first });
	}

	return result;
}

}
