#pragma once

#include "crawler_request.h"
#include "parsed_page.h"
#include "crawler_shared_state.h"

namespace CrawlerEngine
{

class ILicenseStateObserver;

struct RefreshUrlRequest
{
	CrawlerRequest crawlerRequest;
	std::vector<bool> storagesBeforeRemoving;
};

//
// ATTENTION: all public methods must be thread-safe
//

class UniqueLinkStore : public QObject
{
	Q_OBJECT

public:
	UniqueLinkStore(QObject* parent);

	bool extractUrl(CrawlerRequest& crawlerRequest) noexcept;
	bool extractRefreshUrl(RefreshUrlRequest& crawlerRequest) noexcept;
	void addRefreshUrl(const Url& url, DownloadRequestType requestType, const std::vector<bool>& storagesBeforeRemoving);
	void addUrl(const Url& url, DownloadRequestType requestType);
	void addUrl(Url&& url, DownloadRequestType requestType);

	void addUrlList(std::vector<Url> urlList, DownloadRequestType requestType);
	void addLinkList(std::vector<LinkInfo> linkList, DownloadRequestType requestType);

	bool addCrawledUrl(const Url& url, DownloadRequestType requestType);
	bool hasCrawledRequest(const CrawlerRequest& request);

	void activeRequestReceived(const CrawlerRequest& request);

	size_t crawledCount() const noexcept;
	size_t pendingCount() const noexcept;
	size_t activeUrlCount() const noexcept;
	std::vector<CrawlerRequest> crawledUrls() const;
	std::vector<CrawlerRequest> pendingUrls() const;
	std::vector<CrawlerRequest> pendingAndActiveUrls() const;
	void setCrawledUrls(const std::vector<CrawlerRequest>& urls);
	void setPendingUrls(const std::vector<CrawlerRequest>& urls);
	void clear();
	void clearPending();
	bool hasRefreshUrls() const noexcept;
	void setLimitCrawledLinksCount(int value) noexcept;

signals:
	void urlAdded();

private:
	void addUrlInternal(CrawlerRequest&& request);

private:
	struct UrlListItemHasher
	{
		size_t operator()(const CrawlerRequest& item) const noexcept
		{
			return hasher(item.url.urlStr().toStdString()) + static_cast<size_t>(item.requestType);
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
	UrlList m_activeUrlList;
	std::deque<RefreshUrlRequest> m_refreshUrlList;

	mutable std::recursive_mutex m_mutex;
	int m_lastPendingSizeChange;
	int m_lastCrawledSizeChange;

	int m_limitCrawledLinksCount;

	ILicenseStateObserver* m_licenseService;
};

}
