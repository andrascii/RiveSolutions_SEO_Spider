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

	bool extractUrl(CrawlerRequest& url) noexcept;

	void saveUrlList(const std::vector<QUrl>& urlList, DownloadRequestType requestType) noexcept;
	void saveUrlList(std::vector<QUrl>&& urlList, DownloadRequestType requestType) noexcept;

	void saveLinkList(const std::vector<LinkInfo>& linkList, DownloadRequestType requestType) noexcept;

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

	std::unordered_set<CrawlerRequest, UrlListItemHasher> m_pendingUrlList;

	std::unordered_set<CrawlerRequest, UrlListItemHasher> m_crawledUrlList;

	mutable std::mutex m_mutex;
};

}
