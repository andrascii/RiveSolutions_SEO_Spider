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

	bool extractUrl(CrawlerRequest& url);

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
		size_t operator()(const CrawlerRequest& item) const
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

	std::unordered_set<CrawlerRequest, UrlListItemHasher, UrlListItemComparator> m_pendingUrlList;
	std::unordered_set<CrawlerRequest, UrlListItemHasher, UrlListItemComparator> m_crawledUrlList;

	mutable std::mutex m_mutex;
};

}
