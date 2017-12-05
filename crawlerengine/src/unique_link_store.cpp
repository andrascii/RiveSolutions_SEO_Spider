#include "unique_link_store.h"

namespace CrawlerEngine
{

UniqueLinkStore::UniqueLinkStore(QObject* parent)
	: QObject(parent)
	, m_crawledSize(0)
	, m_pendingSize(0)
{
}

void UniqueLinkStore::addUrl(const QUrl& url, DownloadRequestType requestType)
{
	std::lock_guard<std::recursive_mutex> locker(m_mutex);
	IncrementGuard guard(m_pendingSize, m_pendingUrlList);

	CrawlerRequest request{ url, requestType };

	if (m_crawledUrlList.find(request) == m_crawledUrlList.end())
	{
		m_pendingUrlList.insert(std::move(request));

		emit urlAdded();
	}
}

void UniqueLinkStore::addUrl(QUrl&& url, DownloadRequestType requestType)
{
	std::lock_guard<std::recursive_mutex> locker(m_mutex);
	IncrementGuard guard(m_pendingSize, m_pendingUrlList);

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

	IncrementGuard guardCrawled(m_crawledSize, m_crawledUrlList);
	IncrementGuard guardPending(m_pendingSize, m_pendingUrlList);
	m_crawledUrlList.insert(std::move(*iter));
	m_pendingUrlList.erase(iter);

	return true;
}

void UniqueLinkStore::saveUrlList(const std::vector<QUrl>& urlList, DownloadRequestType requestType)
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

void UniqueLinkStore::saveUrlList(std::vector<QUrl>&& urlList, DownloadRequestType requestType)
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
	const auto makeUrlList = [&linkList]() -> std::vector<QUrl>
	{
		std::vector<QUrl> urlList;

		for(const LinkInfo& link : linkList)
		{
			urlList.push_back(link.url);
		}

		return urlList;
	};

	std::vector<QUrl> urlList = makeUrlList();

	saveUrlList(std::move(urlList), requestType);
}

size_t UniqueLinkStore::crawledLinksCount() const noexcept
{
	return m_crawledSize;
}

size_t UniqueLinkStore::pendingLinksCount() const noexcept
{
	return m_pendingSize;
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

void UniqueLinkStore::clear()
{
	std::lock_guard<std::recursive_mutex> locker(m_mutex);

	m_pendingUrlList.clear();
	m_crawledUrlList.clear();

	m_crawledSize.store(0);
	m_pendingSize.store(0);
}

}