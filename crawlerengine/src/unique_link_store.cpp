#include "unique_link_store.h"

namespace CrawlerEngine
{

UniqueLinkStore::UniqueLinkStore(QObject* parent)
	: QObject(parent)
{
}

void UniqueLinkStore::addUrl(const QUrl& url, DownloadRequestType requestType)
{
	std::lock_guard<std::mutex> locker(m_mutex);

	CrawlerRequest item{ url, requestType };

	if (m_crawledUrlList.find(item) == m_crawledUrlList.end())
	{
		m_pendingUrlList.insert(item);
		emit urlAdded();
	}
}

void UniqueLinkStore::addUrl(QUrl&& url, DownloadRequestType requestType)
{
	std::lock_guard<std::mutex> locker(m_mutex);

	CrawlerRequest item{ std::move(url), requestType };

	if (m_crawledUrlList.find(item) == m_crawledUrlList.end())
	{
		m_pendingUrlList.insert(item);
		emit urlAdded();
	}
}

bool UniqueLinkStore::extractUrl(CrawlerRequest& url)
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

void UniqueLinkStore::saveUrlList(const std::vector<QUrl>& urlList, DownloadRequestType requestType)
{
	if (urlList.empty())
	{
		return;
	}

	std::lock_guard<std::mutex> locker(m_mutex);

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

	std::lock_guard<std::mutex> locker(m_mutex);

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
	std::lock_guard<std::mutex> locker(m_mutex);
	return m_crawledUrlList.size();
}

size_t UniqueLinkStore::pendingLinksCount() const noexcept
{
	std::lock_guard<std::mutex> locker(m_mutex);
	return m_pendingUrlList.size();
}

void UniqueLinkStore::clear()
{
	std::lock_guard<std::mutex> locker(m_mutex);

	m_pendingUrlList.clear();
	m_crawledUrlList.clear();
}

}