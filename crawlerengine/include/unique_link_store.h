#pragma once

#include "crawler_request.h"
#include "parsed_page.h"

namespace CrawlerEngine
{

//
// ATTENTION: all public method must be thread-safe
//

class UniqueLinkStore : public QObject
{
	Q_OBJECT

public:
	UniqueLinkStore(QObject* parent);

	void addUrl(const QUrl& url, DownloadRequestType requestType);
	void addUrl(QUrl&& url, DownloadRequestType requestType);

	bool extractUrl(CrawlerRequest& url) noexcept;

	void saveUrlList(const std::vector<QUrl>& urlList, DownloadRequestType requestType);
	void saveUrlList(std::vector<QUrl>&& urlList, DownloadRequestType requestType);
	void saveLinkList(const std::vector<LinkInfo>& linkList, DownloadRequestType requestType);

	size_t crawledLinksCount() const noexcept;
	size_t pendingLinksCount() const noexcept;

	void clear();

signals:
	void urlAdded();

private:
	struct UrlListItemHasher
	{
		size_t operator()(const CrawlerRequest& item) const noexcept
		{
			return hasher(item.url.toString().toStdString()) + static_cast<size_t>(item.requestType);
		}

		boost::hash<std::string> hasher;
	};

	struct UrlListItemComparator
	{
		bool operator()(const CrawlerRequest& lhs, const CrawlerRequest& rhs) const
		{
			return lhs == rhs;
		}
	};

	using UrlList = std::unordered_set<CrawlerRequest, UrlListItemHasher, UrlListItemComparator>;

	struct IncrementGuard
	{
		IncrementGuard(std::atomic<size_t>& size, const UrlList& storage)
			: size(size)
			, storage(storage)
			, oldSize(storage.size())
		{
		}

		~IncrementGuard()
		{
			const size_t newSize = storage.size();
			if (newSize > oldSize)
			{
				++size;
			} 
			else if (newSize < oldSize)
			{
				--size;
			}
		}

		std::atomic<size_t>& size;
		size_t oldSize;
		const UrlList& storage;
	};

	UrlList m_pendingUrlList;
	UrlList m_crawledUrlList;

	mutable std::recursive_mutex m_mutex;

	std::atomic<size_t> m_crawledSize;
	std::atomic<size_t> m_pendingSize;
};

}
