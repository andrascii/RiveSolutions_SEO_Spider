#pragma once

#include "parsed_page.h"
#include "robots_txt_rules.h"
#include "requester_wrapper.h"
#include "crawler_request.h"
#include "worker_result.h"

namespace CrawlerEngine
{

struct CrawlerOptions;
struct DownloadResponse;
class IQueuedDownloader;
class UniqueLinkStore;
class OptionsLinkFilter;
class PageDataCollector;

class CrawlerWorkerThread : public QObject
{
	Q_OBJECT

public:
	CrawlerWorkerThread(UniqueLinkStore* uniqueLinkStore);
	std::future<std::optional<CrawlerRequest>> pendingUrls() const;

signals:
	void workerResult(WorkerResult workerResult) const;

public slots:
	void startWithOptions(const CrawlerOptions& options, RobotsTxtRules robotsTxtRules);
	void stop();

private slots:
	void extractUrlAndDownload();
	void onCrawlerClearData();

private:
	std::vector<LinkInfo> schedulePageResourcesLoading(ParsedPagePtr& parsedPage);
	std::vector<LinkInfo> handlePageLinkList(std::vector<LinkInfo>& linkList, const MetaRobotsFlagsSet& metaRobotsFlags, ParsedPagePtr& parsedPage);
	void onLoadingDone(Requester* requester, const DownloadResponse& response);
	void onStart();
	void onPageParsed(const WorkerResult& result) const noexcept;

	std::optional<CrawlerRequest> prepareUnloadedPage() const;

private:
	struct PagesAcceptedAfterStop
	{
		using PageRequestPair = std::pair<DownloadRequestType, ParsedPagePtr>;
		using PageRequestPairs = std::vector<PageRequestPair>;

		PageRequestPairs pages;
		mutable std::promise<std::optional<CrawlerRequest>> pagesAcceptedPromise;
	};

	PageDataCollector* m_pageDataCollector;
	UniqueLinkStore* m_uniqueLinkStore;

	std::unique_ptr<OptionsLinkFilter> m_optionsLinkFilter;
	RequesterWrapper m_downloadRequester;

	bool m_isRunning;
	bool m_reloadPage;

	PagesAcceptedAfterStop m_pagesAcceptedAfterStop;
	std::optional<CrawlerRequest> m_currentRequest;

	QTimer* m_defferedProcessingTimer;
};

}
