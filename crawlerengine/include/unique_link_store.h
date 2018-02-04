#pragma once

#include "crawler_request.h"
#include "parsed_page.h"
#include "crawler_shared_state.h"

namespace CrawlerEngine
{

//
// ATTENTION: all public methods must be thread-safe
//

class UniqueLinkStore : public QObject
{
	Q_OBJECT

public:
	UniqueLinkStore(QObject* parent);

	bool extractUrl(CrawlerRequest& crawlerRequest) noexcept;
	bool extractRefreshUrl(CrawlerRequest& crawlerRequest) noexcept;
	void addRefreshUrl(const Url& url, DownloadRequestType requestType);
	void addUrl(const Url& url, DownloadRequestType requestType);
	void addUrl(Url&& url, DownloadRequestType requestType);
	void addUrlList(const std::vector<Url>& urlList, DownloadRequestType requestType);
	void addUrlList(std::vector<Url>&& urlList, DownloadRequestType requestType);
	void addLinkList(const std::vector<LinkInfo>& linkList, DownloadRequestType requestType);
	bool addCrawledUrl(const Url& url, DownloadRequestType requestType);

	std::vector<CrawlerRequest> crawledUrls() const noexcept;
	std::vector<CrawlerRequest> pendingUrls() const noexcept;
	void setCrawledUrls(const std::vector<CrawlerRequest>& urls);
	void setPendingUrls(const std::vector<CrawlerRequest>& urls);
	void clear();
	bool hasRefreshUrls() const noexcept;

signals:
	void urlAdded();

private:
	struct UrlListItemHasher
	{
		size_t operator()(const CrawlerRequest& item) const noexcept
		{
			return hasher(item.url.canonizedUrlStr().toStdString()) + static_cast<size_t>(item.requestType);
		}

		boost::hash<std::string> hasher;
	};

	using UrlList = std::unordered_set<CrawlerRequest, UrlListItemHasher>;
	using IncrementFunc = void(CrawlerSharedState::*)() noexcept;

	struct IncrementGuardExt
	{
		IncrementGuardExt(IncrementFunc inc, IncrementFunc decr, const UrlList& storage, int* change = nullptr);
		~IncrementGuardExt();

		IncrementFunc inc;
		IncrementFunc decr;
		size_t oldSize;
		const UrlList& storage;
		int* change;
	};

	UrlList m_pendingUrlList;
	UrlList m_crawledUrlList;
	std::queue<CrawlerRequest> m_refreshUrlQueue;

	mutable std::recursive_mutex m_mutex;
	int m_lastPendingSizeChange;
	int m_lastCrawledSizeChange;
};

}
