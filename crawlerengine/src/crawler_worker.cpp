#include "crawler_worker.h"
#include "unique_link_store.h"
#include "page_parser_helpers.h"
#include "page_data_collector.h"
#include "download_request.h"
#include "download_response.h"
#include "license_state_observer.h"
#include "service_locator.h"
#include "common_constants.h"
#include "crawler.h"
#include "finally.h"
#include "iworker_page_loader.h"

Q_DECLARE_METATYPE(std::vector<bool>)

namespace CrawlerEngine
{

CrawlerWorker::CrawlerWorker(UniqueLinkStore* uniqueLinkStore, IWorkerPageLoader* pageLoader)
	: QObject(nullptr)
	, m_pageDataCollector(new PageDataCollector(this))
	, m_uniqueLinkStore(uniqueLinkStore)
	, m_isRunning(false)
	, m_defferedProcessingTimer(new QTimer(this))
	, m_licenseService(nullptr)
	, m_pageLoader(pageLoader)
{
	m_pageLoader->qobject()->setParent(this);

	qRegisterMetaType<HopsChain>("HopsChain");
	qRegisterMetaType<std::vector<bool>>("std::vector<bool>");
	qRegisterMetaType<DownloadRequestType>("DownloadRequestType");

	VERIFY(connect(m_pageLoader->qobject(), SIGNAL(pageLoaded(const HopsChain&, int, bool, const std::vector<bool>&, DownloadRequestType)),
		this, SLOT(onLoadingDone(const HopsChain&, int, bool, const std::vector<bool>&, DownloadRequestType)), Qt::QueuedConnection));

	VERIFY(connect(m_uniqueLinkStore, &UniqueLinkStore::urlAdded, this,
		&CrawlerWorker::extractUrlAndDownload, Qt::QueuedConnection));

	VERIFY(connect(&Crawler::instance(), &Crawler::onAboutClearData,
		this, &CrawlerWorker::onAllLoadedDataToBeCleared, Qt::QueuedConnection));

	VERIFY(connect(m_defferedProcessingTimer, &QTimer::timeout,
		this, &CrawlerWorker::extractUrlAndDownload));

	m_defferedProcessingTimer->setInterval(1000);
	m_defferedProcessingTimer->setSingleShot(true);

	ASSERT(ServiceLocator::instance()->isRegistered<ILicenseStateObserver>());
	m_licenseService = ServiceLocator::instance()->service<ILicenseStateObserver>();
}

void CrawlerWorker::start(const CrawlerOptionsData& optionsData, RobotsTxtRules robotsTxtRules)
{
	DEBUG_ASSERT(thread() == QThread::currentThread());

	m_isRunning = true;
	m_optionsData = optionsData;

	m_pageLoader->setReceiveState(IWorkerPageLoader::CanReceivePages);

	reinitOptions(optionsData, robotsTxtRules);
	extractUrlAndDownload();
}

void CrawlerWorker::stop()
{
	DEBUG_ASSERT(thread() == QThread::currentThread());

	m_isRunning = false;

	m_pageLoader->setReceiveState(IWorkerPageLoader::CantReceivePages);
}

void CrawlerWorker::reinitOptions(const CrawlerOptionsData& optionsData, RobotsTxtRules robotsTxtRules)
{
	DEBUG_ASSERT(thread() == QThread::currentThread());

	m_optionsLinkFilter.reset(new OptionsLinkFilter(optionsData, robotsTxtRules));
	m_pageDataCollector->setOptions(optionsData);
}

void CrawlerWorker::extractUrlAndDownload()
{
	if (!m_isRunning && !m_uniqueLinkStore->hasRefreshUrls())
	{
		return;
	}

	CrawlerRequest crawlerRequest;
	RefreshUrlRequest refreshRequest;

	bool isUrlExtracted = false;
	bool reloadPage = false;

	reloadPage = isUrlExtracted = m_uniqueLinkStore->extractRefreshUrl(refreshRequest);

	if ((!m_pageLoader->canPullLoading() || !m_isRunning) && !reloadPage)
	{
		m_defferedProcessingTimer->start();
		return;
	}

	if (!isUrlExtracted)
	{
		isUrlExtracted = m_uniqueLinkStore->extractUrl(crawlerRequest);
	}

	if (isUrlExtracted)
	{
		const DownloadRequest::Status linkStatus = reloadPage ?
			DownloadRequest::Status::LinkStatusReloadAlreadyLoaded :
			DownloadRequest::Status::LinkStatusFirstLoading;

		std::vector<bool> reloadingPageStorages;

		if (reloadPage)
		{
			reloadingPageStorages = refreshRequest.storagesBeforeRemoving;
		}

		const CrawlerRequest& request = reloadPage ? refreshRequest.crawlerRequest : crawlerRequest;

		m_pageLoader->performLoading(request, CrawlerSharedState::instance()->turnaround(), reloadingPageStorages, linkStatus);
	}
}

void CrawlerWorker::onAllLoadedDataToBeCleared()
{
	m_pageLoader->clear();
	CrawlerSharedState::instance()->setWorkersProcessedLinksCount(0);
}

CrawlerWorker::SchedulePagesResult CrawlerWorker::schedulePageResourcesLoading(ParsedPagePtr& parsedPage)
{
	SchedulePagesResult result;

	if (parsedPage->statusCode >= Common::StatusCode::BadRequest400)
	{
		return result;
	}

	if (parsedPage->isThisExternalPage)
	{
		if (parsedPage->redirectedUrl.isValid())
		{
			const LinkInfo redirectLinkInfo
			{
				parsedPage->redirectedUrl,
				LinkParameter::DofollowParameter,
				QString(),
				false,
				ResourceSource::SourceRedirectUrl
			};

			const ResourceOnPage redirectedResource(parsedPage->resourceType, redirectLinkInfo);
			parsedPage->allResourcesOnPage.clear();
			parsedPage->allResourcesOnPage.insert(redirectedResource);
		}

		return result;
	}

	std::vector<ResourceOnPage> outlinks;

	for (const ResourceOnPage& resource : parsedPage->allResourcesOnPage)
	{
		if (resource.resourceType == ResourceType::ResourceHtml)
		{
			outlinks.push_back(resource);
		}
	}

	if (parsedPage->redirectedUrl.isValid())
	{
		const LinkInfo redirectLinkInfo
		{
			parsedPage->redirectedUrl,
			LinkParameter::DofollowParameter,
			QString(),
			false,
			ResourceSource::SourceRedirectUrl
		};

		const ResourceOnPage redirectedResource(parsedPage->resourceType, redirectLinkInfo);
		parsedPage->allResourcesOnPage.erase(redirectedResource);
		parsedPage->allResourcesOnPage.insert(redirectedResource);
	}

	result = std::move(handlePageLinkList(outlinks, parsedPage->metaRobotsFlags, parsedPage));
	std::vector<LinkInfo> getHtmlLinks;
	std::transform(outlinks.begin(), outlinks.end(), std::back_inserter(getHtmlLinks), [](const ResourceOnPage& resource)
	{
		return resource.link;
	});

	m_uniqueLinkStore->addLinkList(std::move(getHtmlLinks), DownloadRequestType::RequestTypeGet);

	std::vector<Url> resourcesHeadUrlList;
	std::vector<Url> resourcesGetUrlList;

	for (const ResourceOnPage& resource : parsedPage->allResourcesOnPage)
	{
		if (PageParserHelpers::isHttpOrHttpsScheme(resource.link.url) &&
			resource.resourceType != ResourceType::ResourceHtml &&
			!resource.restrictions)
		{
			if (m_optionsData.limitMaxUrlLength > 0 && resource.link.url.toDisplayString().length() > m_optionsData.limitMaxUrlLength)
			{
				result.tooLongLinks.push_back(resource);
				continue;
			}

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

	m_uniqueLinkStore->addUrlList(std::move(resourcesGetUrlList), DownloadRequestType::RequestTypeGet);
	m_uniqueLinkStore->addUrlList(std::move(resourcesHeadUrlList), DownloadRequestType::RequestTypeHead);

	return result;
}

CrawlerWorker::SchedulePagesResult
CrawlerWorker::handlePageLinkList(std::vector<ResourceOnPage>& linkList, const MetaRobotsFlagsSet& metaRobotsFlags, ParsedPagePtr& parsedPage)
{
	const auto isNofollowLinkUnavailable = [optionsLinkFilter = m_optionsLinkFilter.get(), metaRobotsFlags](const ResourceOnPage& resource)
	{
		return optionsLinkFilter->checkRestriction(Restriction::RestrictionNofollowNotAllowed, resource.link, metaRobotsFlags);
	};

	const auto isLinkBlockedByRobotsTxt = [optionsLinkFilter = m_optionsLinkFilter.get(), metaRobotsFlags](const ResourceOnPage& resource)
	{
		return optionsLinkFilter->checkRestriction(Restriction::RestrictionBlockedByRobotsTxtRules, resource.link, metaRobotsFlags);
	};

	const auto isLinkBlockedByMetaRobots = [optionsLinkFilter = m_optionsLinkFilter.get(), metaRobotsFlags](const ResourceOnPage& resource)
	{
		return optionsLinkFilter->checkRestriction(Restriction::RestrictionBlockedByMetaRobotsRules, resource.link, metaRobotsFlags);
	};

	const auto isSubdomainLinkUnavailable = [optionsLinkFilter = m_optionsLinkFilter.get(), metaRobotsFlags](const ResourceOnPage& resource)
	{
		return optionsLinkFilter->checkRestriction(Restriction::RestrictionSubdomainNotAllowed, resource.link, metaRobotsFlags);
	};

	const auto isExternalLinkUnavailable = [optionsLinkFilter = m_optionsLinkFilter.get(), metaRobotsFlags](const ResourceOnPage& resource)
	{
		return optionsLinkFilter->checkRestriction(Restriction::RestrictionExternalLinksNotAllowed, resource.link, metaRobotsFlags);
	};

	const auto isOutsideFolderLinkUnavailable = [optionsLinkFilter = m_optionsLinkFilter.get(), metaRobotsFlags](const ResourceOnPage& resource)
	{
		return optionsLinkFilter->checkRestriction(Restriction::RestrictionBlockedByFolder, resource.link, metaRobotsFlags);
	};

	const auto setResourceRestrictions = [&](ResourceOnPage& resource)
	{
		if (!PageParserHelpers::isHttpOrHttpsScheme(resource.link.url))
		{
			resource.restrictions.setFlag(Restriction::RestrictionNotHttpLinkNotAllowed, true);
		}

		if (isLinkBlockedByRobotsTxt(resource))
		{
			resource.restrictions.setFlag(Restriction::RestrictionBlockedByRobotsTxtRules, true);
		}

		if (isLinkBlockedByMetaRobots(resource))
		{
			resource.restrictions.setFlag(Restriction::RestrictionBlockedByMetaRobotsRules, true);
		}

		if (isNofollowLinkUnavailable(resource))
		{
			resource.restrictions.setFlag(Restriction::RestrictionNofollowNotAllowed, true);
		}

		if (isSubdomainLinkUnavailable(resource))
		{
			resource.restrictions.setFlag(Restriction::RestrictionSubdomainNotAllowed, true);
		}

		if (isExternalLinkUnavailable(resource))
		{
			resource.restrictions.setFlag(Restriction::RestrictionExternalLinksNotAllowed, true);
		}

		if (isOutsideFolderLinkUnavailable(resource))
		{
			resource.restrictions.setFlag(Restriction::RestrictionBlockedByFolder, true);
		}
	};

	const auto isTooLongLink = [this](const ResourceOnPage& resource)
	{
		return m_optionsData.limitMaxUrlLength == 0 ? false : resource.link.url.toDisplayString().length() > m_optionsData.limitMaxUrlLength;
	};

	SchedulePagesResult result;

	linkList.erase(std::remove_if(linkList.begin(), linkList.end(), isLinkBlockedByMetaRobots), linkList.end());
	linkList.erase(std::remove_if(linkList.begin(), linkList.end(), isNofollowLinkUnavailable), linkList.end());
	linkList.erase(std::remove_if(linkList.begin(), linkList.end(), isSubdomainLinkUnavailable), linkList.end());
	linkList.erase(std::remove_if(linkList.begin(), linkList.end(), isExternalLinkUnavailable), linkList.end());
	linkList.erase(std::remove_if(linkList.begin(), linkList.end(), isOutsideFolderLinkUnavailable), linkList.end());

	const auto blockedByRobotsTxtLinksIterator = std::remove_if(linkList.begin(), linkList.end(), isLinkBlockedByRobotsTxt);
	std::copy(blockedByRobotsTxtLinksIterator, linkList.end(), std::back_inserter(result.blockedByRobotsTxtLinks));
	linkList.erase(blockedByRobotsTxtLinksIterator, linkList.end());

	const auto tooLongLinksIt = std::remove_if(linkList.begin(), linkList.end(), isTooLongLink);
	std::copy(tooLongLinksIt, linkList.end(), std::back_inserter(result.tooLongLinks));
	linkList.erase(tooLongLinksIt, linkList.end());

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

	return result;
}

void CrawlerWorker::onLoadingDone(const HopsChain& hopsChain,
	int turnaround,
	bool isPageReloaded,
	const std::vector<bool>& reloadingPageStrorages,
	DownloadRequestType requestType)
{
	if (CrawlerSharedState::instance()->turnaround() != turnaround)
	{
		return;
	}

	CrawlerRequest readyRequest = { hopsChain.firstHop().url(), requestType };
	m_uniqueLinkStore->activeRequestReceived(readyRequest);

	extractUrlAndDownload();

	DEBUG_ASSERT(requestType != DownloadRequestType::RequestTypeHead || hopsChain.firstHop().body().isEmpty());

	handleResponseData(hopsChain, turnaround, isPageReloaded, reloadingPageStrorages, requestType);
}

void CrawlerWorker::onPageParsed(const WorkerResult& result) const noexcept
{
	if (result.incomingPageConstRef()->isRedirectedToExternalPage())
	{
		DEBUG_ASSERT(result.incomingPageConstRef()->allResourcesOnPage.size() == 1 &&
			!result.incomingPageConstRef()->allResourcesOnPage.begin()->restrictions);
	}

	DEBUG_ASSERT(!result.incomingPageConstRef()->redirectedUrl.isValid() ||
		!result.incomingPageConstRef()->isThisExternalPage ||
		result.incomingPageConstRef()->allResourcesOnPage.size() == 1);

	emit workerResult(result);

	CrawlerSharedState::instance()->incrementWorkersProcessedLinksCount();
}

void CrawlerWorker::fixDDOSGuardRedirectsIfNeeded(std::vector<ParsedPagePtr>& pages) const
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

void CrawlerWorker::handlePage(ParsedPagePtr& page,
	int turnaround,
	bool isStoredInCrawledUrls,
	bool isPageReloaded,
	const std::vector<bool>& reloadingPageStrorages,
	DownloadRequestType requestType)
{
	const auto emitBlockedByRobotsTxtPages = [this, turnaround](const ResourceOnPage& resource)
	{
		if (m_uniqueLinkStore->addCrawledUrl(resource.link.url, DownloadRequestType::RequestTypeGet))
		{
			ParsedPagePtr page(new ParsedPage);

			page->url = resource.link.url;
			page->statusCode = Common::StatusCode::BlockedByRobotsTxt;
			page->resourceType = ResourceType::ResourceHtml;

			onPageParsed(WorkerResult{ page, turnaround, false, DownloadRequestType::RequestTypeHead, std::vector<bool>() });
		}
	};

	const auto emitTooLongLinksPages = [this, turnaround](const ResourceOnPage& resource)
	{
		if (m_uniqueLinkStore->addCrawledUrl(resource.link.url, DownloadRequestType::RequestTypeGet))
		{
			ParsedPagePtr page(new ParsedPage);

			page->url = resource.link.url;
			page->statusCode = Common::StatusCode::TooLongLink;
			page->resourceType = resource.resourceType;

			onPageParsed(WorkerResult{ page, turnaround, false, DownloadRequestType::RequestTypeHead, std::vector<bool>() });
		}
	};

	if (isStoredInCrawledUrls)
	{
		SchedulePagesResult readyLinks = schedulePageResourcesLoading(page);

		const std::pair<bool, MetaRobotsFlags> isPageBlockedByMetaRobots = m_optionsLinkFilter->isPageBlockedByMetaRobots(page);

		if (isPageBlockedByMetaRobots.first)
		{
			page->isBlockedByMetaRobots = isPageBlockedByMetaRobots.second.testFlag(MetaRobotsItem::MetaRobotsNoIndex);
		}

		onPageParsed(WorkerResult{ page, turnaround, isPageReloaded, requestType, reloadingPageStrorages });

		std::for_each(readyLinks.blockedByRobotsTxtLinks.begin(), readyLinks.blockedByRobotsTxtLinks.end(), emitBlockedByRobotsTxtPages);
		std::for_each(readyLinks.tooLongLinks.begin(), readyLinks.tooLongLinks.end(), emitTooLongLinksPages);
	}
}

void CrawlerWorker::handleResponseData(const HopsChain& hopsChain,
	int turnaround,
	bool isPageReloaded,
	const std::vector<bool>& reloadingPageStrorages,
	DownloadRequestType requestType)
{
	std::vector<ParsedPagePtr> pages = m_pageDataCollector->collectPageDataFromResponse(hopsChain);

	for (size_t i = 0; i < pages.size() - 1; ++i)
	{
		// fix resource type in redirects chain
		pages[i]->resourceType = pages.back()->resourceType;
	}

	bool checkUrl = false;

	fixDDOSGuardRedirectsIfNeeded(pages);

	for (ParsedPagePtr& page : pages)
	{
		if (checkUrl && !page->url.fragment().isEmpty())
		{
			page->url.setFragment(QString());
		}
		const bool isUrlAdded = !checkUrl || m_uniqueLinkStore->addCrawledUrl(page->url, requestType);

		handlePage(page, turnaround, isUrlAdded, isPageReloaded, reloadingPageStrorages, requestType);

		checkUrl = true;
	}
}

}
