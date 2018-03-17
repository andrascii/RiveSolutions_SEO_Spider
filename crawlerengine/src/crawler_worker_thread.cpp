#include "crawler_worker_thread.h"
#include "unique_link_store.h"
#include "page_parser_helpers.h"
#include "page_data_collector.h"
#include "options_link_filter.h"
#include "download_request.h"
#include "download_response.h"
#include "crawler.h"

namespace
{

template <typename T>
class StoreValueGuard final
{
public:
	StoreValueGuard(T& ref, T newValue)
		: m_ref(ref)
		, m_newValue(newValue)
	{
	}

	~StoreValueGuard()
	{
		m_ref = m_newValue;
	}

private:
	T& m_ref;
	T m_newValue;
};

}

namespace CrawlerEngine
{

CrawlerWorkerThread::CrawlerWorkerThread(UniqueLinkStore* uniqueLinkStore)
	: QObject(nullptr)
	, m_pageDataCollector(new PageDataCollector(this))
	, m_uniqueLinkStore(uniqueLinkStore)
	, m_isRunning(false)
	, m_reloadPage(false)
	, m_defferedProcessingTimer(new QTimer(this))
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

std::future<std::optional<CrawlerRequest>> CrawlerWorkerThread::pendingUrls() const
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
	const int modelControllerCrawledLinksCount = state->modelControllerCrawledLinksCount();

	const int pendingLinksCount = workersProcessedLinksCount - modelControllerCrawledLinksCount;
	constexpr int maxPendingLinksCount = 15;

	if (pendingLinksCount > maxPendingLinksCount)
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
		DownloadRequest request(crawlerRequest, m_reloadPage);
		m_currentRequest = crawlerRequest;
		m_downloadRequester.reset(request, this, &CrawlerWorkerThread::onLoadingDone);
		m_downloadRequester->start();
	}
}

void CrawlerWorkerThread::onCrawlerClearData()
{
	m_pagesAcceptedAfterStop.pages.clear();
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
			const ResourceOnPage redirectedResource(parsedPage->resourceType, redirectLinkInfo, Permission::PermissionAllowed);
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

	outlinks = PageParserHelpers::resolveUrlList(parsedPage->url, outlinks);

	std::vector<LinkInfo> blockedByRobotsTxtLinks = handlePageLinkList(outlinks, parsedPage->metaRobotsFlags, parsedPage);

	m_uniqueLinkStore->addLinkList(std::move(outlinks), DownloadRequestType::RequestTypeGet);

	if (parsedPage->redirectedUrl.isValid())
	{
		const LinkInfo redirectLinkInfo{ parsedPage->redirectedUrl, LinkParameter::DofollowParameter, QString(), false, ResourceSource::SourceRedirectUrl };
		const ResourceOnPage redirectedResource(parsedPage->resourceType, redirectLinkInfo, Permission::PermissionAllowed);
		parsedPage->allResourcesOnPage.erase(redirectedResource);
		parsedPage->allResourcesOnPage.insert(redirectedResource);
	}

	std::vector<Url> resourcesHeadUrlList;
	std::vector<Url> resourcesGetUrlList;

	for (const ResourceOnPage& resource : parsedPage->allResourcesOnPage)
	{
		if (PageParserHelpers::isHttpOrHttpsScheme(resource.link.url) &&
			resource.resourceType != ResourceType::ResourceHtml &&
			resource.permission == Permission::PermissionAllowed)
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
		return optionsLinkFilter->checkPermissionNotAllowed(Permission::PermissionNofollowNotAllowed, linkInfo, metaRobotsFlags);
	};

	const auto isLinkBlockedByRobotsTxt = [optionsLinkFilter = m_optionsLinkFilter.get(), metaRobotsFlags](const LinkInfo& linkInfo)
	{
		return optionsLinkFilter->checkPermissionNotAllowed(Permission::PermissionBlockedByRobotsTxtRules, linkInfo, metaRobotsFlags);
	};

	const auto isLinkBlockedByMetaRobots = [optionsLinkFilter = m_optionsLinkFilter.get(), metaRobotsFlags](const LinkInfo& linkInfo)
	{
		return optionsLinkFilter->checkPermissionNotAllowed(Permission::PermissionBlockedByMetaRobotsRules, linkInfo, metaRobotsFlags);
	};

	const auto isSubdomainLinkUnavailable = [optionsLinkFilter = m_optionsLinkFilter.get(), metaRobotsFlags](const LinkInfo& linkInfo)
	{
		return optionsLinkFilter->checkPermissionNotAllowed(Permission::PermissionSubdomainNotAllowed, linkInfo, metaRobotsFlags);
	};

	const auto isExternalLinkUnavailable = [optionsLinkFilter = m_optionsLinkFilter.get(), metaRobotsFlags](const LinkInfo& linkInfo)
	{
		return optionsLinkFilter->checkPermissionNotAllowed(Permission::PermissionExternalLinksNotAllowed, linkInfo, metaRobotsFlags);
	};

	const auto isOutsideFolderLinkUnavailable = [optionsLinkFilter = m_optionsLinkFilter.get(), metaRobotsFlags](const LinkInfo& linkInfo)
	{
		return optionsLinkFilter->checkPermissionNotAllowed(Permission::PermissionBlockedByFolder, linkInfo, metaRobotsFlags);
	};

	//
	// TODO: make it based on flags
	//
	const auto setResourcePermission = [&](ResourceOnPage& resource)
	{
		if (!PageParserHelpers::isHttpOrHttpsScheme(resource.link.url))
		{
			resource.permission = Permission::PermissionNotHttpLinkNotAllowed;
		}
		else if (isLinkBlockedByRobotsTxt(resource.link))
		{
			resource.permission = Permission::PermissionBlockedByRobotsTxtRules;
		}
		else if (isLinkBlockedByMetaRobots(resource.link))
		{
			resource.permission = Permission::PermissionBlockedByMetaRobotsRules;
		}
		else if (isNofollowLinkUnavailable(resource.link))
		{
			resource.permission = Permission::PermissionNofollowNotAllowed;
		}
		else if (isSubdomainLinkUnavailable(resource.link))
		{
			resource.permission = Permission::PermissionSubdomainNotAllowed;
		}
		else if (isExternalLinkUnavailable(resource.link))
		{
			resource.permission = Permission::PermissionExternalLinksNotAllowed;
		}
		else if (isOutsideFolderLinkUnavailable(resource.link))
		{
			resource.permission = Permission::PermissionBlockedByFolder;
		}
		else
		{
			resource.permission = Permission::PermissionAllowed;
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

	for (const ResourceOnPage& resource : parsedPage->allResourcesOnPage)
	{
		ResourceOnPage fixedResource = resource;
		setResourcePermission(fixedResource);

		if (fixedResource.permission == Permission::PermissionBlockedByMetaRobotsRules)
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
	StoreValueGuard<bool> reloadGuard(m_reloadPage, false);

	m_downloadRequester.reset();

	std::vector<ParsedPagePtr> pages = m_pageDataCollector->collectPageDataFromResponse(response);

	ASSERT(m_currentRequest.has_value());
	const DownloadRequestType requestType = m_currentRequest.value().requestType;

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

	bool checkUrl = false;

	// fix ddos guard redirects like page.html -> ddos-site -> page.html
	for (int i = static_cast<int>(pages.size()) - 1; i >= 0; --i)
	{
		for (int j = i - 1; j >= 0; --j)
		{
			if (pages[i]->url.urlStr() == pages[j]->url.urlStr())
			{
				const Url redirectUrl = pages[j]->redirectedUrl;
				*pages[j] = *pages[i];
				pages[j]->redirectedUrl = redirectUrl;
			}
		}
	}

	for (ParsedPagePtr& page : pages)
	{
		const bool urlAdded = !checkUrl || m_uniqueLinkStore->addCrawledUrl(page->url, requestType);

		if (urlAdded && !m_isRunning && !m_reloadPage)
		{
			m_pagesAcceptedAfterStop.pages.push_back(std::make_pair(m_currentRequest.value().requestType, page));

			continue;
		}

		if (urlAdded)
		{
			std::vector<LinkInfo> blockedByRobotsTxtLinks = schedulePageResourcesLoading(page);
			onPageParsed(WorkerResult{ page, m_reloadPage, requestType });
			std::for_each(blockedByRobotsTxtLinks.begin(), blockedByRobotsTxtLinks.end(), emitBlockedByRobotsTxtPages);
		}

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

	extractUrlAndDownload();
}

void CrawlerWorkerThread::onStart()
{
	if (!m_pagesAcceptedAfterStop.pages.empty())
	{
		for (const PagesAcceptedAfterStop::PageRequestPair& pair : m_pagesAcceptedAfterStop.pages)
		{
			onPageParsed(WorkerResult{ pair.second, false, pair.first });
		}
	}

	m_pagesAcceptedAfterStop.pages.clear();

	m_pagesAcceptedAfterStop.pagesAcceptedPromise = std::promise<std::optional<CrawlerRequest>>();
}

void CrawlerWorkerThread::onPageParsed(const WorkerResult& result) const noexcept
{
	if (result.incomingPageConstRef()->redirectedUrl.isValid() && result.incomingPageConstRef()->isThisExternalPage)
	{
		DEBUG_ASSERT(result.incomingPageConstRef()->allResourcesOnPage.size() == 1 &&
			result.incomingPageConstRef()->allResourcesOnPage.begin()->permission == Permission::PermissionAllowed);
	}

	emit workerResult(result);

	CrawlerSharedState::instance()->incrementWorkersProcessedLinksCount();
}

std::optional<CrawlerRequest> CrawlerWorkerThread::prepareUnloadedPage() const
{
	CrawlerRequest result;
	
	if (!m_pagesAcceptedAfterStop.pages.empty())
	{
		PagesAcceptedAfterStop::PageRequestPair pair = m_pagesAcceptedAfterStop.pages.front();

		return CrawlerRequest{ pair.second->url, pair.first };
	}

	return std::nullopt;
}

}
