#include "crawler_worker_thread.h"
#include "unique_link_store.h"
#include "page_parser_helpers.h"
#include "page_data_collector.h"
#include "options_link_filter.h"
#include "download_request.h"
#include "download_response.h"
#include "license_service.h"
#include "service_locator.h"
#include "common_constants.h"
#include "crawler.h"
#include "finally.h"

namespace CrawlerEngine
{

std::atomic<size_t> CrawlerWorkerThread::s_trialLicenseSentLinksCounter = 0;

CrawlerWorkerThread::CrawlerWorkerThread(UniqueLinkStore* uniqueLinkStore)
	: QObject(nullptr)
	, m_pageDataCollector(new PageDataCollector(this))
	, m_uniqueLinkStore(uniqueLinkStore)
	, m_isRunning(false)
	, m_reloadPage(false)
	, m_defferedProcessingTimer(new QTimer(this))
	, m_licenseService(nullptr)
{
	VERIFY(connect(m_uniqueLinkStore, &UniqueLinkStore::urlAdded, this,
		&CrawlerWorkerThread::extractUrlAndDownload, Qt::QueuedConnection));

	VERIFY(connect(&Crawler::instance(), &Crawler::onAboutClearData, 
		this, &CrawlerWorkerThread::onCrawlerClearData, Qt::QueuedConnection));

	VERIFY(connect(m_defferedProcessingTimer, &QTimer::timeout, 
		this, &CrawlerWorkerThread::extractUrlAndDownload));
	
	m_defferedProcessingTimer->setInterval(1000);
	m_defferedProcessingTimer->setSingleShot(true);

	ASSERT(ServiceLocator::instance()->isRegistered<ILicenseService>());
	m_licenseService = ServiceLocator::instance()->service<ILicenseService>();
}

/// this function always must be thread-safe
std::optional<CrawlerRequest> CrawlerWorkerThread::pendingUrl() const
{
	std::optional<CrawlerRequest> value;

	try
	{
		std::future<std::optional<CrawlerRequest>> future = m_pagesAcceptedAfterStop.pagesAcceptedPromise.get_future();
		std::future_status status = future.wait_for(0ms);

		if (status == std::future_status::ready)
		{
			value = future.get();
		}
	}
	catch (const std::future_error& fe)
	{
		WARNLOG << fe.what();
	}

	return value;
}

void CrawlerWorkerThread::start(const CrawlerOptionsData& optionsData, RobotsTxtRules robotsTxtRules)
{
	DEBUG_ASSERT(thread() == QThread::currentThread());

	m_isRunning = true;

	reinitOptions(optionsData, robotsTxtRules);
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

			m_pagesAcceptedAfterStop.pagesAcceptedPromise.set_value(prepareUnloadedPage());
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

void CrawlerWorkerThread::reinitOptions(const CrawlerOptionsData& optionsData, RobotsTxtRules robotsTxtRules)
{
	DEBUG_ASSERT(thread() == QThread::currentThread());

	if (m_optionsLinkFilter && optionsData.startCrawlingPage.compare(m_optionsLinkFilter->startCrawlingPage()))
	{
		s_trialLicenseSentLinksCounter = 0;
	}

	m_optionsLinkFilter.reset(new OptionsLinkFilter(optionsData, robotsTxtRules));
	m_pageDataCollector->setOptions(optionsData);
}

void CrawlerWorkerThread::extractUrlAndDownload()
{
	if (!m_isRunning && !m_uniqueLinkStore->hasRefreshUrls())
	{
		return;
	}

	if (m_downloadRequester)
	{
		return;
	}

	const CrawlerSharedState* state = CrawlerSharedState::instance();
	const int workersProcessedLinksCount = state->workersProcessedLinksCount();
	const int donwloaderCrawledLinksCOunt = state->downloaderCrawledLinksCount();
 
	const int differenceBetweenWorkersAndDownloader = donwloaderCrawledLinksCOunt - workersProcessedLinksCount;
	constexpr int maxPendingLinksCount = 50;

	if (differenceBetweenWorkersAndDownloader > maxPendingLinksCount)
	{
		m_defferedProcessingTimer->start();
		return;
	}

	CrawlerRequest crawlerRequest;
	bool isUrlExtracted = false;

	m_reloadPage = isUrlExtracted = m_uniqueLinkStore->extractRefreshUrl(crawlerRequest);

	if (!m_isRunning && !m_reloadPage)
	{
		return;
	}

	if (!isUrlExtracted)
	{
		isUrlExtracted = m_uniqueLinkStore->extractUrl(crawlerRequest);
	}

	if (isUrlExtracted)
	{
		DownloadRequest request(crawlerRequest, 
			m_reloadPage ? 
			DownloadRequest::LinkStatus::LinkStatusReloadAlreadyLoaded : 
			DownloadRequest::LinkStatus::LinkStatusFirstLoading,
			DownloadRequest::BodyProcessingCommand::CommandAutoDetectionBodyLoadingNecessity,
			true
		);

		m_currentRequest = crawlerRequest;
		m_downloadRequester.reset(request, this, &CrawlerWorkerThread::onLoadingDone);
		m_downloadRequester->start();
	}
}

void CrawlerWorkerThread::onCrawlerClearData()
{
	m_pagesAcceptedAfterStop.pages.clear();
	m_pagesAcceptedAfterStop.pagesAcceptedPromise = std::promise<std::optional<CrawlerRequest>>();
	m_downloadRequester.reset();
	m_currentRequest.reset();
	CrawlerSharedState::instance()->setWorkersProcessedLinksCount(0);
}

std::vector<LinkInfo> CrawlerWorkerThread::schedulePageResourcesLoading(ParsedPagePtr& parsedPage)
{
	if (parsedPage->isThisExternalPage)
	{
		if (parsedPage->redirectedUrl.isValid())
		{
			const LinkInfo redirectLinkInfo{ parsedPage->redirectedUrl, LinkParameter::DofollowParameter, QString(), false, ResourceSource::SourceRedirectUrl };
			const ResourceOnPage redirectedResource(parsedPage->resourceType, redirectLinkInfo);
			parsedPage->allResourcesOnPage.clear();
			parsedPage->allResourcesOnPage.insert(redirectedResource);
		}

		return std::vector<LinkInfo>();
	}

	std::vector<LinkInfo> outlinks;

	for (const ResourceOnPage& resource : parsedPage->allResourcesOnPage)
	{
		if (resource.resourceType == ResourceType::ResourceHtml)
		{
			outlinks.push_back(resource.link);
		}
	}

	if (parsedPage->redirectedUrl.isValid())
	{
		const LinkInfo redirectLinkInfo{ parsedPage->redirectedUrl, LinkParameter::DofollowParameter, QString(), false, ResourceSource::SourceRedirectUrl };
		const ResourceOnPage redirectedResource(parsedPage->resourceType, redirectLinkInfo);
		parsedPage->allResourcesOnPage.erase(redirectedResource);
		parsedPage->allResourcesOnPage.insert(redirectedResource);
	}

	PageParserHelpers::resolveUrlList(parsedPage->baseUrl, outlinks);

	std::vector<LinkInfo> blockedByRobotsTxtLinks = handlePageLinkList(outlinks, parsedPage->metaRobotsFlags, parsedPage);

	m_uniqueLinkStore->addLinkList(std::move(outlinks), DownloadRequestType::RequestTypeGet);

	std::vector<Url> resourcesHeadUrlList;
	std::vector<Url> resourcesGetUrlList;

	for (const ResourceOnPage& resource : parsedPage->allResourcesOnPage)
	{
		if (PageParserHelpers::isHttpOrHttpsScheme(resource.link.url) &&
			resource.resourceType != ResourceType::ResourceHtml &&
			!resource.restrictions)
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

	return blockedByRobotsTxtLinks;
}

std::vector<LinkInfo> CrawlerWorkerThread::handlePageLinkList(std::vector<LinkInfo>& linkList, const MetaRobotsFlagsSet& metaRobotsFlags, ParsedPagePtr& parsedPage)
{
	const auto isNofollowLinkUnavailable = [optionsLinkFilter = m_optionsLinkFilter.get(), metaRobotsFlags](const LinkInfo& linkInfo)
	{
		return optionsLinkFilter->checkRestriction(Restriction::RestrictionNofollowNotAllowed, linkInfo, metaRobotsFlags);
	};

	const auto isLinkBlockedByRobotsTxt = [optionsLinkFilter = m_optionsLinkFilter.get(), metaRobotsFlags](const LinkInfo& linkInfo)
	{
		return optionsLinkFilter->checkRestriction(Restriction::RestrictionBlockedByRobotsTxtRules, linkInfo, metaRobotsFlags);
	};

	const auto isLinkBlockedByMetaRobots = [optionsLinkFilter = m_optionsLinkFilter.get(), metaRobotsFlags](const LinkInfo& linkInfo)
	{
		return optionsLinkFilter->checkRestriction(Restriction::RestrictionBlockedByMetaRobotsRules, linkInfo, metaRobotsFlags);
	};

	const auto isSubdomainLinkUnavailable = [optionsLinkFilter = m_optionsLinkFilter.get(), metaRobotsFlags](const LinkInfo& linkInfo)
	{
		return optionsLinkFilter->checkRestriction(Restriction::RestrictionSubdomainNotAllowed, linkInfo, metaRobotsFlags);
	};

	const auto isExternalLinkUnavailable = [optionsLinkFilter = m_optionsLinkFilter.get(), metaRobotsFlags](const LinkInfo& linkInfo)
	{
		return optionsLinkFilter->checkRestriction(Restriction::RestrictionExternalLinksNotAllowed, linkInfo, metaRobotsFlags);
	};

	const auto isOutsideFolderLinkUnavailable = [optionsLinkFilter = m_optionsLinkFilter.get(), metaRobotsFlags](const LinkInfo& linkInfo)
	{
		return optionsLinkFilter->checkRestriction(Restriction::RestrictionBlockedByFolder, linkInfo, metaRobotsFlags);
	};

	const auto setResourceRestrictions = [&](ResourceOnPage& resource)
	{
		if (!PageParserHelpers::isHttpOrHttpsScheme(resource.link.url))
		{
			resource.restrictions.setFlag(Restriction::RestrictionNotHttpLinkNotAllowed, true);
		}

		if (isLinkBlockedByRobotsTxt(resource.link))
		{
			resource.restrictions.setFlag(Restriction::RestrictionBlockedByRobotsTxtRules, true);
		}

		if (isLinkBlockedByMetaRobots(resource.link))
		{
			resource.restrictions.setFlag(Restriction::RestrictionBlockedByMetaRobotsRules, true);
		}

		if (isNofollowLinkUnavailable(resource.link))
		{
			resource.restrictions.setFlag(Restriction::RestrictionNofollowNotAllowed, true);
		}

		if (isSubdomainLinkUnavailable(resource.link))
		{
			resource.restrictions.setFlag(Restriction::RestrictionSubdomainNotAllowed, true);
		}

		if (isExternalLinkUnavailable(resource.link))
		{
			resource.restrictions.setFlag(Restriction::RestrictionExternalLinksNotAllowed, true);
		}

		if (isOutsideFolderLinkUnavailable(resource.link))
		{
			resource.restrictions.setFlag(Restriction::RestrictionBlockedByFolder, true);
		}
	};

	std::vector<LinkInfo> blockedByRobotsTxtLinks;

	linkList.erase(std::remove_if(linkList.begin(), linkList.end(), isLinkBlockedByMetaRobots), linkList.end());
	linkList.erase(std::remove_if(linkList.begin(), linkList.end(), isNofollowLinkUnavailable), linkList.end());
	linkList.erase(std::remove_if(linkList.begin(), linkList.end(), isSubdomainLinkUnavailable), linkList.end());
	linkList.erase(std::remove_if(linkList.begin(), linkList.end(), isExternalLinkUnavailable), linkList.end());
	linkList.erase(std::remove_if(linkList.begin(), linkList.end(), isOutsideFolderLinkUnavailable), linkList.end());

	const auto blockedByRobotsTxtLinksIterator = std::remove_if(linkList.begin(), linkList.end(), isLinkBlockedByRobotsTxt);
	std::copy(blockedByRobotsTxtLinksIterator, linkList.end(), std::back_inserter(blockedByRobotsTxtLinks));
	linkList.erase(blockedByRobotsTxtLinksIterator, linkList.end());

	ResourcesOnPageList resources;

	bool isLinksOnPageBlockedByMetaRobots = false;
	const std::pair<bool, MetaRobotsFlags> isPageBlockedByMetaRobots = m_optionsLinkFilter->isPageBlockedByMetaRobots(parsedPage);

	if (isPageBlockedByMetaRobots.first)
	{
		isLinksOnPageBlockedByMetaRobots = isPageBlockedByMetaRobots.second.testFlag(MetaRobotsItem::MetaRobotsNoFollow);
	}

	for (const ResourceOnPage& resource : parsedPage->allResourcesOnPage)
	{
		ResourceOnPage fixedResource = resource;
		setResourceRestrictions(fixedResource);

		if (isPageBlockedByMetaRobots.first && isLinksOnPageBlockedByMetaRobots)
		{
			fixedResource.link.linkParameter = LinkParameter::NofollowParameter;
		}

		resources.insert(fixedResource);
	}

	parsedPage->allResourcesOnPage = resources;

	return blockedByRobotsTxtLinks;
}

void CrawlerWorkerThread::onLoadingDone(Requester*, const DownloadResponse& response)
{
	Common::Finally reloadGuard([this]
	{ 
		m_reloadPage = false; 
	});

	m_downloadRequester.reset();

	ASSERT(m_currentRequest.has_value());
	const DownloadRequestType requestType = m_currentRequest.value().requestType;

	handleResponseData(response.hopsChain, requestType);
	extractUrlAndDownload();
}

void CrawlerWorkerThread::onStart()
{
	if (!m_pagesAcceptedAfterStop.pages.empty())
	{
		for (const auto& pair : m_pagesAcceptedAfterStop.pages)
		{
			onPageParsed(WorkerResult{ pair.second, false, pair.first });
		}
	}

	m_pagesAcceptedAfterStop.pages.clear();

	m_pagesAcceptedAfterStop.pagesAcceptedPromise = std::promise<std::optional<CrawlerRequest>>();
}

void CrawlerWorkerThread::onPageParsed(const WorkerResult& result) const noexcept
{
	if (result.incomingPageConstRef()->isRedirectedToExternalPage())
	{
		DEBUG_ASSERT(result.incomingPageConstRef()->allResourcesOnPage.size() == 1 &&
			!result.incomingPageConstRef()->allResourcesOnPage.begin()->restrictions);
	}

	if (m_licenseService->isTrialLicense() && s_trialLicenseSentLinksCounter++ >= Common::c_maxTrialLicenseCrawlingLinksCount)
	{
		m_uniqueLinkStore->clearPending();

		return;
	}

	emit workerResult(result);

	CrawlerSharedState::instance()->incrementWorkersProcessedLinksCount();
}

void CrawlerWorkerThread::fixDDOSGuardRedirectsIfNeeded(std::vector<ParsedPagePtr>& pages) const
{

	const int pagesCount = static_cast<int>(pages.size());

	for (int i = pagesCount - 1; i >= 0; --i)
	{
		// fix ddos guard redirects like page.html -> ddos-site -> page.html
		for (int j = i - 1; j >= 0; --j)
		{
			if (pages[i]->url.urlStr() == pages[j]->url.urlStr())
			{
				const Url redirectUrl = pages[j]->redirectedUrl;

				std::swap(*pages[j], *pages[i]);

				pages[j]->redirectedUrl = redirectUrl;
			}
		}
	}
}

std::optional<CrawlerRequest> CrawlerWorkerThread::prepareUnloadedPage() const
{
	CrawlerRequest result;
	
	if (!m_pagesAcceptedAfterStop.pages.empty())
	{
		const auto pair = m_pagesAcceptedAfterStop.pages.front();

		return CrawlerRequest{ pair.second->url, pair.first };
	}

	return std::nullopt;
}

void CrawlerWorkerThread::handlePage(ParsedPagePtr& page, bool isStoredInCrawledUrls, DownloadRequestType requestType)
{
	const auto emitBlockedByRobotsTxtPages = [this](const LinkInfo& linkInfo)
	{
		if (m_uniqueLinkStore->addCrawledUrl(linkInfo.url, DownloadRequestType::RequestTypeGet))
		{
			ParsedPagePtr page(new ParsedPage);

			page->url = linkInfo.url;
			page->statusCode = Common::StatusCode::BlockedByRobotsTxt;
			page->resourceType = ResourceType::ResourceHtml;

			onPageParsed(WorkerResult{ page, false, DownloadRequestType::RequestTypeHead });
		}
	};

	if (isStoredInCrawledUrls && !m_isRunning && !m_reloadPage)
	{
		m_pagesAcceptedAfterStop.pages.push_back(std::make_pair(m_currentRequest.value().requestType, page));

		return;
	}

	if (isStoredInCrawledUrls)
	{
		std::vector<LinkInfo> blockedByRobotsTxtLinks = schedulePageResourcesLoading(page);

		const std::pair<bool, MetaRobotsFlags> isPageBlockedByMetaRobots = m_optionsLinkFilter->isPageBlockedByMetaRobots(page);

		if (isPageBlockedByMetaRobots.first)
		{
			page->isBlockedByMetaRobots = isPageBlockedByMetaRobots.second.testFlag(MetaRobotsItem::MetaRobotsNoIndex);
		}

		onPageParsed(WorkerResult{ page, m_reloadPage, requestType });

		std::for_each(blockedByRobotsTxtLinks.begin(), blockedByRobotsTxtLinks.end(), emitBlockedByRobotsTxtPages);
	}
}

void CrawlerWorkerThread::handleResponseData(const HopsChain& hopsChain, DownloadRequestType requestType)
{
	std::vector<ParsedPagePtr> pages = m_pageDataCollector->collectPageDataFromResponse(hopsChain);

	bool checkUrl = false;

	fixDDOSGuardRedirectsIfNeeded(pages);

	for (ParsedPagePtr& page : pages)
	{
		const bool isUrlAdded = !checkUrl || m_uniqueLinkStore->addCrawledUrl(page->url, requestType);

		handlePage(page, isUrlAdded, requestType);

		checkUrl = true;
	}

	if (!m_isRunning && !m_reloadPage)
	{
		DEBUGLOG << "Set value to promise";

		try
		{
			m_pagesAcceptedAfterStop.pagesAcceptedPromise.set_value(prepareUnloadedPage());
		}
		catch (const std::future_error& error)
		{
			if (error.code() == std::make_error_condition(std::future_errc::promise_already_satisfied))
			{
				DEBUG_ASSERT(!"At this stage promise must not be valid! Possibly problem in downloader!");
			}
			else
			{
				throw;
			}
		}
	}
}

}
