#pragma once

#include "crawler_request.h"

namespace WebCrawler
{

class CrawlerUrlStorage
{
public:
	// all methods are thread-safe
	void setHost(const QUrl& url);

	// returns random url for crawling (async operation)
	// returns false if urls queue is empty
	// otherwise returns true and write url to passed argument
	bool extractUrl(CrawlerRequest& url) noexcept;

	void saveUrlList(const std::vector<QUrl>& urlList, RequestType requestType) noexcept;

	size_t crawledLinksCount() const noexcept;
	size_t pendingLinksCount() const noexcept;

private:
	struct UrlListItemHasher
	{
		size_t operator()(const CrawlerRequest& item) const noexcept
		{
			return hasher(item.url.toString().toStdString()) + static_cast<size_t>(item.requestType);
		}

		boost::hash<std::string> hasher;
	};

	std::unordered_set<CrawlerRequest, UrlListItemHasher> m_internalUrlList;
	std::unordered_set<CrawlerRequest, UrlListItemHasher> m_crawledUrlList;

	mutable std::mutex m_mutex;
};

}
