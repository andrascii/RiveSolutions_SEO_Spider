#include "unique_link_store.h"

namespace CrawlerEngine
{

UniqueLinkStore::UniqueLinkStore(QObject* parent)
	: QObject(parent)
{
}

void UniqueLinkStore::addUrl(const QUrl& url, RequestType requestType)
{
	std::lock_guard<std::mutex> locker(m_mutex);

	m_pendingUrlList.insert(CrawlerRequest{ url, requestType });

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

	return true;
}

void UniqueLinkStore::saveUrlList(const std::vector<QUrl>& urlList, RequestType requestType) noexcept
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
			++insertedUrlCounter;
		}
	};

	for (auto first = urlList.begin(), last = urlList.end(); first != last; ++first)
	{
		insert(first);
	}

	emit urlAdded();
}

void UniqueLinkStore::saveLinkList(const std::vector<LinkInfo>& linkList, RequestType requestType) noexcept
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

	const std::vector<QUrl> urlList = makeUrlList();

	saveUrlList(urlList, requestType);
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