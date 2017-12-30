#pragma once

#include "crawler_request.h"
#include "parsed_page.h"
#include "crawler_shared_state.h"

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

	bool extractUrl(CrawlerRequest& url) noexcept;

	void addUrl(const Url& url, DownloadRequestType requestType);
	void addUrl(Url&& url, DownloadRequestType requestType);
	void addUrlList(const std::vector<Url>& urlList, DownloadRequestType requestType);
	void addUrlList(std::vector<Url>&& urlList, DownloadRequestType requestType);
	void addLinkList(const std::vector<LinkInfo>& linkList, DownloadRequestType requestType);

	std::vector<CrawlerRequest> crawledUrls() const noexcept;
	std::vector<CrawlerRequest> pendingUrls() const noexcept;
	void setCrawledUrls(const std::vector<CrawlerRequest>& urls);
	void setPendingUrls(const std::vector<CrawlerRequest>& urls);
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

	using UrlList = std::unordered_set<CrawlerRequest, UrlListItemHasher>;
	using IncrementFunc = void(CrawlerSharedState::*)() noexcept;

	struct IncrementGuardExt
	{
		IncrementGuardExt(IncrementFunc inc, IncrementFunc decr, const UrlList& storage);
		~IncrementGuardExt();

		IncrementFunc inc;
		IncrementFunc decr;
		size_t oldSize;
		const UrlList& storage;
	};

	UrlList m_pendingUrlList;
	UrlList m_crawledUrlList;

	mutable std::recursive_mutex m_mutex;
};

}
