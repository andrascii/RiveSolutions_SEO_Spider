#pragma once
#include "web_crawler_request.h"

namespace WebCrawler
{

class WebCrawlerInternalUrlStorage
{
public:
	// all methods are thread-safe

	void setHost(const QUrl& url);

	// returns random url for crawling (async operation)
	// returns false if urls queue is empty
	// otherwise returns true and write url to passed argument
	bool extractUrl(WebCrawlerRequest& url) noexcept;

	void saveUrlList(const std::vector<QUrl>& urlList, RequestType requestType) noexcept;

private:
	struct UrlListItemHasher
	{
		size_t operator()(const WebCrawlerRequest& item) const noexcept
		{
			return hasher(item.url.toString().toStdString()) + static_cast<size_t>(item.requestType);
		}

		boost::hash<std::string> hasher;
	};

	std::unordered_set<WebCrawlerRequest, UrlListItemHasher> m_internalUrlList;
	std::unordered_set<WebCrawlerRequest, UrlListItemHasher> m_crawledUrlList;

	std::mutex m_mutex;
};

}
