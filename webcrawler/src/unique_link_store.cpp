#include "unique_link_store.h"

namespace WebCrawler
{

void UniqueLinkStore::setHost(const QUrl& url)
{
	std::lock_guard<std::mutex> locker(m_mutex);

	m_pendingUrlList.insert(CrawlerRequest{ url, RequestTypeGet });
}

bool UniqueLinkStore::extractUrl(CrawlerRequest& url) noexcept
{
	std::lock_guard<std::mutex> locker(m_mutex);

	if (m_pendingUrlList.empty())
	{
		return false;
	}

	const auto iter = m_pendingUrlList.begin();
	url = *iter;

	m_crawledUrlList.insert(*iter);
	m_pendingUrlList.erase(iter);

	return true;
}

void UniqueLinkStore::saveUrlList(const std::vector<QUrl>& urlList, RequestType requestType) noexcept
{
	using VectorIterator = std::vector<QUrl>::const_iterator;

	if (urlList.empty())
	{
		return;
	}

	std::lock_guard<std::mutex> locker(m_mutex);

	size_t insertedUrlCounter = 0;

	const auto insert = [&](VectorIterator iter)
	{
		CrawlerRequest item{ *iter, requestType };

		if (m_crawledUrlList.find(item) == m_crawledUrlList.end())
		{
			m_pendingUrlList.insert(item);
			++insertedUrlCounter;
		}
	};

	for (VectorIterator first = urlList.begin(), last = urlList.end(); first != last; ++first)
	{
		insert(first);
	}
}


size_t UniqueLinkStore::crawledLinksCount() const noexcept
{
	std::lock_guard<std::mutex> locker(m_mutex);
	return m_crawledUrlList.size();
}

size_t UniqueLinkStore::pendingLinksCount() const noexcept
{
	std::lock_guard<std::mutex> locker(m_mutex);
	return m_pendingUrlList.size();
}

}