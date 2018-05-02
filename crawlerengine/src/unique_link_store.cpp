#include "unique_link_store.h"

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
{
}

void UniqueLinkStore::addUrl(const Url& url, DownloadRequestType requestType)
{
	std::lock_guard<std::recursive_mutex> locker(m_mutex);

	IncrementGuardExt guardPendingExt(&CrawlerSharedState::incrementDownloaderPendingLinksCount,
		&CrawlerSharedState::decrementDownloaderPendingLinksCount, m_pendingUrlList, &m_lastPendingSizeChange);

	CrawlerRequest request{ url, requestType };

	if (m_crawledUrlList.find(request) == m_crawledUrlList.end())
	{
		if (request.url.urlStr() == QString("http://www.cyberforum.ru/javascript-beginners/"))
		{
			INFOLOG << request.url.urlStr();
		}

		m_pendingUrlList.insert(std::move(request));

		emit urlAdded();
	}
}

void UniqueLinkStore::addUrl(Url&& url, DownloadRequestType requestType)
{
	std::lock_guard<std::recursive_mutex> locker(m_mutex);

	IncrementGuardExt guardPendingExt(&CrawlerSharedState::incrementDownloaderPendingLinksCount,
		&CrawlerSharedState::decrementDownloaderPendingLinksCount, m_pendingUrlList, &m_lastPendingSizeChange);

	CrawlerRequest request{ std::move(url), requestType };

	if (m_crawledUrlList.find(request) == m_crawledUrlList.end())
	{
		if (request.url.urlStr() == QString("http://www.cyberforum.ru/javascript-beginners/"))
		{
			INFOLOG << request.url.urlStr();
		}

		m_pendingUrlList.insert(std::move(request));

		emit urlAdded();
	}
}

bool UniqueLinkStore::extractUrl(CrawlerRequest& crawlerRequest) noexcept
{
	std::lock_guard<std::recursive_mutex> locker(m_mutex);

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
			INFOLOG << crawlerRequest.url.urlStr() << static_cast<int>(crawlerRequest.requestType);
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
	std::lock_guard<std::recursive_mutex> locker(m_mutex);

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
	std::lock_guard<std::recursive_mutex> locker(m_mutex);

	m_refreshUrlList.emplace_back(CrawlerRequest{ url, requestType });

	emit urlAdded();
}

void UniqueLinkStore::addUrlList(const std::vector<Url>& urlList, DownloadRequestType requestType)
{
	if (urlList.empty())
	{
		return;
	}

	std::lock_guard<std::recursive_mutex> locker(m_mutex);

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

	std::lock_guard<std::recursive_mutex> locker(m_mutex);

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
	std::lock_guard<std::recursive_mutex> locker(m_mutex);

	CrawlerRequest request{ url, requestType };

	{
		IncrementGuardExt guardCrawledExt(&CrawlerSharedState::incrementDownloaderCrawledLinksCount, 
			nullptr, m_crawledUrlList, &m_lastCrawledSizeChange);
		
		IncrementGuardExt guardPendingExt(&CrawlerSharedState::incrementDownloaderPendingLinksCount,
			&CrawlerSharedState::decrementDownloaderPendingLinksCount, m_pendingUrlList, &m_lastPendingSizeChange);

		m_pendingUrlList.erase(request);
		m_crawledUrlList.insert(std::move(request));
	}

	DEBUG_ASSERT(m_lastPendingSizeChange == 0 || m_lastCrawledSizeChange != 0);
	DEBUG_ASSERT(m_lastCrawledSizeChange != 0 || m_lastPendingSizeChange == 0);
	return m_lastCrawledSizeChange != 0 || m_lastPendingSizeChange != 0;
}

bool UniqueLinkStore::hasCrawledRequest(const CrawlerRequest& request)
{
	std::lock_guard<std::recursive_mutex> locker(m_mutex);

	return m_crawledUrlList.find(request) != m_crawledUrlList.end();
}

std::vector<CrawlerRequest> UniqueLinkStore::crawledUrls() const noexcept
{
	std::lock_guard<std::recursive_mutex> locker(m_mutex);
	std::vector<CrawlerRequest> result;
	result.reserve(m_crawledUrlList.size());

	for (auto it = m_crawledUrlList.begin(); it != m_crawledUrlList.end(); ++it)
	{
		result.emplace_back(*it);
	}

	return result;
}

std::vector<CrawlerRequest> UniqueLinkStore::pendingUrls() const noexcept
{
	std::lock_guard<std::recursive_mutex> locker(m_mutex);
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
	std::lock_guard<std::recursive_mutex> locker(m_mutex);
	m_crawledUrlList.clear();
	m_crawledUrlList.insert(urls.begin(), urls.end());
}

void UniqueLinkStore::setPendingUrls(const std::vector<CrawlerRequest>& urls)
{
	std::lock_guard<std::recursive_mutex> locker(m_mutex);
	m_pendingUrlList.clear();
	m_pendingUrlList.insert(urls.begin(), urls.end());
}

void UniqueLinkStore::clear()
{
	std::lock_guard<std::recursive_mutex> locker(m_mutex);

	m_pendingUrlList.clear();
	m_crawledUrlList.clear();
	m_refreshUrlList.clear();

	CrawlerSharedState::instance()->setDownloaderCrawledLinksCount(0);
	CrawlerSharedState::instance()->setDownloaderPendingLinksCount(0);
}

bool UniqueLinkStore::hasRefreshUrls() const noexcept
{
	std::lock_guard<std::recursive_mutex> locker(m_mutex);
	return !m_refreshUrlList.empty();
}

}