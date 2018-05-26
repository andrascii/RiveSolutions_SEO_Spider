#include "unique_link_store.h"
#include "service_locator.h"
#include "license_service.h"
#include "common_constants.h"

namespace CrawlerEngine
{

UniqueLinkStore::IncrementGuardExt::IncrementGuardExt(IncrementFunc inc, 
	IncrementFunc decr, const UrlList& storage, int* change)
	: inc(inc)
	, decr(decr)
	, storage(storage)
	, oldSize(storage.size())
	, change(change)
{
}

UniqueLinkStore::IncrementGuardExt::~IncrementGuardExt()
{
	CrawlerSharedState* state = CrawlerSharedState::instance();

	const size_t newSize = storage.size();
	if (newSize > oldSize)
	{
		(state->*inc)();
	}
	else if (newSize < oldSize)
	{
		ASSERT(decr != nullptr);
		(state->*decr)();
	}

	if (change != nullptr)
	{
		*change = static_cast<int>(newSize) - static_cast<int>(oldSize);
	}
}


UniqueLinkStore::UniqueLinkStore(QObject* parent)
	: QObject(parent)
	, m_licenseService(nullptr)
{
	ASSERT(ServiceLocator::instance()->isRegistered<ILicenseService>());

	m_licenseService = ServiceLocator::instance()->service<ILicenseService>();
}

void UniqueLinkStore::addUrl(const Url& url, DownloadRequestType requestType)
{
	std::lock_guard locker(m_mutex);

	IncrementGuardExt guardPendingExt(&CrawlerSharedState::incrementDownloaderPendingLinksCount,
		&CrawlerSharedState::decrementDownloaderPendingLinksCount, m_pendingUrlList, &m_lastPendingSizeChange);

	addUrlInternal(CrawlerRequest{ url, requestType });
}

void UniqueLinkStore::addUrl(Url&& url, DownloadRequestType requestType)
{
	std::lock_guard locker(m_mutex);

	IncrementGuardExt guardPendingExt(&CrawlerSharedState::incrementDownloaderPendingLinksCount,
		&CrawlerSharedState::decrementDownloaderPendingLinksCount, m_pendingUrlList, &m_lastPendingSizeChange);

	addUrlInternal(CrawlerRequest{ std::move(url), requestType });
}

bool UniqueLinkStore::extractUrl(CrawlerRequest& crawlerRequest) noexcept
{
	std::lock_guard locker(m_mutex);

	if (m_pendingUrlList.empty())
	{
		return false;
	}

	{
		IncrementGuardExt guardCrawledExt(&CrawlerSharedState::incrementDownloaderCrawledLinksCount, 
			nullptr, m_crawledUrlList, &m_lastCrawledSizeChange);
		
		IncrementGuardExt guardPendingExt(&CrawlerSharedState::incrementDownloaderPendingLinksCount,
			&CrawlerSharedState::decrementDownloaderPendingLinksCount, m_pendingUrlList, &m_lastPendingSizeChange);

		const auto iter = m_pendingUrlList.begin();
		crawlerRequest = *iter;

		if (m_crawledUrlList.find(crawlerRequest) != m_crawledUrlList.end())
		{
			INFOLOG << "Url from pending found in crawled url list:" 
				<< crawlerRequest.url.urlStr() 
				<< static_cast<int>(crawlerRequest.requestType);
		}

		DEBUG_ASSERT(m_crawledUrlList.find(crawlerRequest) == m_crawledUrlList.end());
		m_crawledUrlList.insert(std::move(*iter));
		m_pendingUrlList.erase(iter);
	}

	ASSERT(m_lastPendingSizeChange == -1);

	return true;
}

bool UniqueLinkStore::extractRefreshUrl(CrawlerRequest& crawlerRequest) noexcept
{
	std::lock_guard locker(m_mutex);

	if (m_refreshUrlList.empty())
	{
		return false;
	}

	crawlerRequest = std::move(m_refreshUrlList.front());
	m_refreshUrlList.pop_front();

	return true;
}

void UniqueLinkStore::addRefreshUrl(const Url& url, DownloadRequestType requestType)
{
	std::lock_guard locker(m_mutex);

	m_refreshUrlList.emplace_back(CrawlerRequest{ url, requestType });

	emit urlAdded();
}

void UniqueLinkStore::addUrlList(const std::vector<Url>& urlList, DownloadRequestType requestType)
{
	if (urlList.empty())
	{
		return;
	}

	std::lock_guard locker(m_mutex);

	for (auto first = urlList.begin(), last = urlList.end(); first != last; ++first)
	{
		addUrl(*first, requestType);
	}
}

void UniqueLinkStore::addUrlList(std::vector<Url>&& urlList, DownloadRequestType requestType)
{
	if (urlList.empty())
	{
		return;
	}

	std::lock_guard locker(m_mutex);

	for (auto first = urlList.begin(); first != urlList.end(); ++first)
	{
		addUrl(std::move(*first), requestType);
	}
}

void UniqueLinkStore::addLinkList(const std::vector<LinkInfo>& linkList, DownloadRequestType requestType)
{
	const auto makeUrlList = [&linkList]() -> std::vector<Url>
	{
		std::vector<Url> urlList;

		for(const LinkInfo& link : linkList)
		{
			urlList.push_back(link.url);
		}

		return urlList;
	};

	std::vector<Url> urlList = makeUrlList();

	addUrlList(std::move(urlList), requestType);
}

bool UniqueLinkStore::addCrawledUrl(const Url& url, DownloadRequestType requestType)
{
	std::lock_guard locker(m_mutex);

	CrawlerRequest request{ url, requestType };

	{
		IncrementGuardExt guardCrawledExt(&CrawlerSharedState::incrementDownloaderCrawledLinksCount, 
			nullptr, m_crawledUrlList, &m_lastCrawledSizeChange);
		
		IncrementGuardExt guardPendingExt(&CrawlerSharedState::incrementDownloaderPendingLinksCount,
			&CrawlerSharedState::decrementDownloaderPendingLinksCount, m_pendingUrlList, &m_lastPendingSizeChange);

		const auto urlIter = m_pendingUrlList.find(request);

		if (urlIter == m_pendingUrlList.end())
		{
			// The condition is need because of some requests may not be presented in the pending url list.
			// This situation may occur because of worker thread adds links from hops chain.
			const size_t countLinks = m_pendingUrlList.size() + m_crawledUrlList.size();

			if (countLinks < Common::c_maxTrialLicenseCrawlingLinksCount)
			{
				m_crawledUrlList.insert(std::move(request));
			}
		}
		else
		{
			m_pendingUrlList.erase(urlIter);
			m_crawledUrlList.insert(std::move(request));
		}
	}

	DEBUG_ASSERT(m_lastPendingSizeChange == 0 || m_lastCrawledSizeChange != 0);
	DEBUG_ASSERT(m_lastCrawledSizeChange != 0 || m_lastPendingSizeChange == 0);
	return m_lastCrawledSizeChange != 0 || m_lastPendingSizeChange != 0;
}

bool UniqueLinkStore::hasCrawledRequest(const CrawlerRequest& request)
{
	std::lock_guard locker(m_mutex);

	return m_crawledUrlList.find(request) != m_crawledUrlList.end();
}

size_t UniqueLinkStore::crawledCount() const noexcept
{
	std::lock_guard locker(m_mutex);
	return m_crawledUrlList.size();
}

size_t UniqueLinkStore::pendingCount() const noexcept
{
	std::lock_guard locker(m_mutex);
	return m_pendingUrlList.size();
}

std::vector<CrawlerRequest> UniqueLinkStore::crawledUrls() const
{
	std::lock_guard locker(m_mutex);
	std::vector<CrawlerRequest> result;
	result.reserve(m_crawledUrlList.size());

	for (auto it = m_crawledUrlList.begin(); it != m_crawledUrlList.end(); ++it)
	{
		result.emplace_back(*it);
	}

	return result;
}

std::vector<CrawlerRequest> UniqueLinkStore::pendingUrls() const
{
	std::lock_guard locker(m_mutex);
	std::vector<CrawlerRequest> result;
	result.reserve(m_pendingUrlList.size());

	for (auto it = m_pendingUrlList.begin(); it != m_pendingUrlList.end(); ++it)
	{
		result.emplace_back(*it);
	}

	return result;
}

void UniqueLinkStore::setCrawledUrls(const std::vector<CrawlerRequest>& urls)
{
	std::lock_guard locker(m_mutex);
	m_crawledUrlList.clear();
	m_crawledUrlList.insert(urls.begin(), urls.end());
}

void UniqueLinkStore::setPendingUrls(const std::vector<CrawlerRequest>& urls)
{
	std::lock_guard locker(m_mutex);
	m_pendingUrlList.clear();
	m_pendingUrlList.insert(urls.begin(), urls.end());
}

void UniqueLinkStore::clear()
{
	std::lock_guard locker(m_mutex);

	m_pendingUrlList.clear();
	m_crawledUrlList.clear();
	m_refreshUrlList.clear();

	CrawlerSharedState::instance()->setDownloaderCrawledLinksCount(0);
	CrawlerSharedState::instance()->setDownloaderPendingLinksCount(0);
}

void UniqueLinkStore::clearPending()
{
	std::lock_guard locker(m_mutex);
	m_pendingUrlList.clear();
	CrawlerSharedState::instance()->setDownloaderPendingLinksCount(0);
}

bool UniqueLinkStore::hasRefreshUrls() const noexcept
{
	std::lock_guard locker(m_mutex);
	return !m_refreshUrlList.empty();
}

void UniqueLinkStore::addUrlInternal(CrawlerRequest&& request)
{
	// this function must be obfuscated and additionally tied to a serial number
	if (m_crawledUrlList.find(request) != m_crawledUrlList.end())
	{
		return;
	}

	const size_t countLinks = m_pendingUrlList.size() + m_crawledUrlList.size();

	ASSERT(!m_licenseService->isTrialLicense() || countLinks <= Common::c_maxTrialLicenseCrawlingLinksCount);

	if (m_licenseService->isTrialLicense() && countLinks >= Common::c_maxTrialLicenseCrawlingLinksCount)
	{
		return;
	}

	m_pendingUrlList.insert(std::move(request));

	emit urlAdded();
}

}