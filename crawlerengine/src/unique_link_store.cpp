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
	std::lock_guard<std::mutex> locker(m_mutex);

	m_pendingUrlList.insert(CrawlerRequest{ url, requestType });
	++m_pendingSize;

	emit urlAdded();
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

	++m_crawledSize;
	--m_pendingSize;

	return true;
}

void UniqueLinkStore::saveUrlList(const std::vector<QUrl>& urlList, DownloadRequestType requestType) noexcept
{
	if (urlList.empty())
	{
		return;
	}

	std::lock_guard<std::mutex> locker(m_mutex);

	size_t insertedUrlCounter = 0;

	const auto insert = [&](const auto& iter)
	{
		CrawlerRequest item{ *iter, requestType };

		if (m_crawledUrlList.find(item) == m_crawledUrlList.end())
		{
			m_pendingUrlList.insert(item);
			++m_pendingSize;
			++insertedUrlCounter;
		}
	};

	for (auto first = urlList.begin(), last = urlList.end(); first != last; ++first)
	{
		insert(first);
	}

	emit urlAdded();
}

void UniqueLinkStore::saveUrlList(std::vector<QUrl>&& urlList, DownloadRequestType requestType) noexcept
{
	if (urlList.empty())
	{
		return;
	}

	std::lock_guard<std::mutex> locker(m_mutex);

	size_t insertedUrlCounter = 0;

	const auto insert = [&](const auto& iter)
	{
		CrawlerRequest item{ std::move(*iter), requestType };

		if (m_crawledUrlList.find(item) == m_crawledUrlList.end())
		{
			m_pendingUrlList.insert(item);
			++m_pendingSize;
			++insertedUrlCounter;
		}
	};

	for (auto first = urlList.begin(); first != urlList.end(); ++first)
	{
		insert(first);
	}

	emit urlAdded();
}

void UniqueLinkStore::saveLinkList(const std::vector<LinkInfo>& linkList, DownloadRequestType requestType) noexcept
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

void UniqueLinkStore::clear()
{
	std::lock_guard<std::mutex> locker(m_mutex);

	m_pendingUrlList.clear();
	m_crawledUrlList.clear();

	m_crawledSize.store(0);
	m_pendingSize.store(0);
}

}