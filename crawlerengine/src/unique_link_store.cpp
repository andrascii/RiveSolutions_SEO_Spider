#include "unique_link_store.h"

namespace CrawlerEngine
{

UniqueLinkStore::IncrementGuardExt::IncrementGuardExt(IncrementFunc inc, IncrementFunc decr, const UrlList& storage)
	: inc(inc)
	, decr(decr)
	, storage(storage)
	, oldSize(storage.size())
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
}


UniqueLinkStore::UniqueLinkStore(QObject* parent)
	: QObject(parent)
{
}

void UniqueLinkStore::addUrl(const CustomUrl& url, DownloadRequestType requestType)
{
	std::lock_guard<std::recursive_mutex> locker(m_mutex);

	IncrementGuardExt guardPendingExt(&CrawlerSharedState::incrementDownloaderPendingLinksCount,
		&CrawlerSharedState::decrementDownloaderPendingLinksCount, m_pendingUrlList);

	CrawlerRequest request{ url, requestType };

	if (m_crawledUrlList.find(request) == m_crawledUrlList.end())
	{
		m_pendingUrlList.insert(std::move(request));

		emit urlAdded();
	}
}

void UniqueLinkStore::addUrl(CustomUrl&& url, DownloadRequestType requestType)
{
	std::lock_guard<std::recursive_mutex> locker(m_mutex);
	IncrementGuardExt guardPendingExt(&CrawlerSharedState::incrementDownloaderPendingLinksCount,
		&CrawlerSharedState::decrementDownloaderPendingLinksCount, m_pendingUrlList);

	CrawlerRequest request{ std::move(url), requestType };

	if (m_crawledUrlList.find(request) == m_crawledUrlList.end())
	{
		m_pendingUrlList.insert(std::move(request));

		emit urlAdded();
	}
}

bool UniqueLinkStore::extractUrl(CrawlerRequest& url) noexcept
{
	std::lock_guard<std::recursive_mutex> locker(m_mutex);

	if (m_pendingUrlList.empty())
	{
		return false;
	}

	const auto iter = m_pendingUrlList.begin();
	url = *iter;

	IncrementGuardExt guardCrawledExt(&CrawlerSharedState::incrementDownloaderCrawledLinksCount, nullptr, m_crawledUrlList);
	IncrementGuardExt guardPendingExt(&CrawlerSharedState::incrementDownloaderPendingLinksCount, 
		&CrawlerSharedState::decrementDownloaderPendingLinksCount, m_pendingUrlList);

	m_crawledUrlList.insert(std::move(*iter));
	m_pendingUrlList.erase(iter);

	return true;
}

void UniqueLinkStore::saveUrlList(const std::vector<CustomUrl>& urlList, DownloadRequestType requestType)
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

void UniqueLinkStore::saveUrlList(std::vector<CustomUrl>&& urlList, DownloadRequestType requestType)
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

void UniqueLinkStore::saveLinkList(const std::vector<LinkInfo>& linkList, DownloadRequestType requestType)
{
	const auto makeUrlList = [&linkList]() -> std::vector<CustomUrl>
	{
		std::vector<CustomUrl> urlList;

		for(const LinkInfo& link : linkList)
		{
			urlList.push_back(link.url);
		}

		return urlList;
	};

	std::vector<CustomUrl> urlList = makeUrlList();

	saveUrlList(std::move(urlList), requestType);
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

	for (auto it = std::cbegin(urls); it != std::cend(urls); ++it)
	{
		m_crawledUrlList.insert(*it);
	}
}

void UniqueLinkStore::setPendingUrls(const std::vector<CrawlerRequest>& urls)
{
	std::lock_guard<std::recursive_mutex> locker(m_mutex);
	m_pendingUrlList.clear();

	for (auto it = std::cbegin(urls); it != std::cend(urls); ++it)
	{
		m_pendingUrlList.insert(*it);
	}
}

void UniqueLinkStore::clear()
{
	std::lock_guard<std::recursive_mutex> locker(m_mutex);

	m_pendingUrlList.clear();
	m_crawledUrlList.clear();

	CrawlerSharedState::instance()->setDownloaderCrawledLinksCount(0);
	CrawlerSharedState::instance()->setDownloaderPendingLinksCount(0);
}

}