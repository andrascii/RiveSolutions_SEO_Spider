#pragma once

#include "crawler_request.h"
#include "parsed_page.h"

namespace CrawlerEngine
{

//
// ATTENTION: all public method must be thread-safe
//

class UniqueLinkStore
{
public:
	void setHost(const QUrl& url);
	bool extractUrl(CrawlerRequest& url) noexcept;
	void saveUrlList(const std::vector<QUrl>& urlList, RequestType requestType) noexcept;
	void saveLinkList(const std::vector<LinkInfo>& linkList, RequestType requestType) noexcept;

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

	std::unordered_set<CrawlerRequest, UrlListItemHasher> m_pendingUrlList;
	std::unordered_set<CrawlerRequest, UrlListItemHasher> m_crawledUrlList;

	mutable std::mutex m_mutex;
};

}
