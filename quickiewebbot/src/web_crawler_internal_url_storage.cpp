#include "web_crawler_internal_url_storage.h"

namespace QuickieWebBot
{

void WebCrawlerInternalUrlStorage::setHost(const QUrl& url)
{
	std::lock_guard<std::mutex> locker(m_mutex);

	m_internalUrlList.insert(url);
}

bool WebCrawlerInternalUrlStorage::get(QUrl& url) noexcept
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

void WebCrawlerInternalUrlStorage::saveUrlList(const std::vector<QUrl>& urlList) noexcept
{
	using VectorIterator = std::vector<QUrl>::const_iterator;

	std::lock_guard<std::mutex> locker(m_mutex);

	size_t insertedUrlCounter = 0;

	auto insert = [&](VectorIterator iter)
	{
		//
		// here two or more threads can attempt insert the same element
		// more then once, but collections are thread-safe
		//
		if (m_crawledUrlList.find(*iter) == m_crawledUrlList.end())
		{
			m_internalUrlList.insert(*iter);
			++insertedUrlCounter;
		}
	};

	for (VectorIterator first = urlList.begin(), last = urlList.end(); first != last; ++first)
	{
		insert(first);
	}

	if (insertedUrlCounter)
	{
		m_condition.notify_all();
	}
}

}