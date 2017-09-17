#include "web_crawler_internal_url_storage.h"

namespace WebCrawler
{

void WebCrawlerInternalUrlStorage::setHost(const QUrl& url)
{
	std::lock_guard<std::mutex> locker(m_mutex);

	m_internalUrlList.insert(WebCrawlerRequest{ url, RequestTypeGet });
}

bool WebCrawlerInternalUrlStorage::extractUrl(WebCrawlerRequest& url) noexcept
{
	std::lock_guard<std::mutex> locker(m_mutex);

	if (m_internalUrlList.empty())
	{
		return false;
	}

	auto iter = m_internalUrlList.begin();
	url = *iter;

	m_crawledUrlList.insert(*iter);
	m_internalUrlList.erase(iter);

	return true;
}

void WebCrawlerInternalUrlStorage::saveUrlList(const std::vector<QUrl>& urlList, RequestType requestType) noexcept
{
	using VectorIterator = std::vector<QUrl>::const_iterator;

	std::lock_guard<std::mutex> locker(m_mutex);

	size_t insertedUrlCounter = 0;

	auto insert = [&](VectorIterator iter)
	{
		WebCrawlerRequest item{ *iter, requestType };

		if (m_crawledUrlList.find(item) == m_crawledUrlList.end())
		{
			m_internalUrlList.insert(item);
			++insertedUrlCounter;
		}
	};

	for (VectorIterator first = urlList.begin(), last = urlList.end(); first != last; ++first)
	{
		insert(first);
	}
}

}