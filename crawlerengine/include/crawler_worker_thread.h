#pragma once

#include "parsed_page.h"
#include "robots_txt_rules.h"
#include "requester_wrapper.h"
#include "crawler_request.h"
#include "worker_result.h"

namespace CrawlerEngine
{

class IQueuedDownloader;
class UniqueLinkStore;
class OptionsLinkFilter;
class PageDataCollector;
struct CrawlerOptionsData;
struct DownloadResponse;

class CrawlerWorkerThread : public QObject
{
	Q_OBJECT

public:
	CrawlerWorkerThread(UniqueLinkStore* uniqueLinkStore);
	std::optional<CrawlerRequest> pendingUrl() const;

signals:
	void workerResult(WorkerResult workerResult) const;

public slots:
	void start(const CrawlerOptionsData& optionsData, RobotsTxtRules robotsTxtRules);

	void stop();

private slots:
	void extractUrlAndDownload();

	void onCrawlerClearData();

private:
	void onStart();

	std::vector<LinkInfo> schedulePageResourcesLoading(ParsedPagePtr& parsedPage);

	std::vector<LinkInfo> handlePageLinkList(std::vector<LinkInfo>& linkList, const MetaRobotsFlagsSet& metaRobotsFlags, ParsedPagePtr& parsedPage);

	void onLoadingDone(Requester* requester, const DownloadResponse& response);

	void onPageParsed(const WorkerResult& result) const noexcept;

	void fixDDOSGuardRedirectsIfNeeded(std::vector<ParsedPagePtr>& pages) const;

	std::optional<CrawlerRequest> prepareUnloadedPage() const;

	void handlePage(ParsedPagePtr& page, bool isUrlAdded, DownloadRequestType requestType);

private:
	struct PagesAcceptedAfterStop
	{
		using PageRequestPairs = std::vector<std::pair<DownloadRequestType, ParsedPagePtr>>;
		using PagesPromise = std::promise<std::optional<CrawlerRequest>>;

		PageRequestPairs pages;
		PagesPromise mutable pagesAcceptedPromise;
	};

	PageDataCollector* m_pageDataCollector;
	UniqueLinkStore* m_uniqueLinkStore;

	std::unique_ptr<OptionsLinkFilter> m_optionsLinkFilter;
	RequesterWrapper m_downloadRequester;

	bool m_isRunning;
	bool m_reloadPage;

	PagesAcceptedAfterStop m_pageAcceptedAfterStop;
	std::optional<CrawlerRequest> m_currentRequest;

	QTimer* m_defferedProcessingTimer;
};

}
