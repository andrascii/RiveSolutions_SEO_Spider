#pragma once

#include "parsed_page.h"
#include "robots_txt_rules.h"
#include "requester_wrapper.h"
#include "crawler_request.h"
#include "worker_result.h"

namespace CrawlerEngine
{

struct CrawlerOptionsData;
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
	std::optional<CrawlerRequest> pendingUrl() const;

signals:
	void workerResult(WorkerResult workerResult) const;

public slots:
	void startWithOptions(const CrawlerOptionsData& optionsData, RobotsTxtRules robotsTxtRules);
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
	struct PageAcceptedAfterStop
	{
		using PageRequestPair = std::pair<DownloadRequestType, ParsedPagePtr>;
		using PageRequestPairs = std::vector<PageRequestPair>;

		PageRequestPairs page;
		mutable std::promise<std::optional<CrawlerRequest>> pageAcceptedPromise;
	};

	PageDataCollector* m_pageDataCollector;
	UniqueLinkStore* m_uniqueLinkStore;

	std::unique_ptr<OptionsLinkFilter> m_optionsLinkFilter;
	RequesterWrapper m_downloadRequester;

	bool m_isRunning;
	bool m_reloadPage;

	PageAcceptedAfterStop m_pageAcceptedAfterStop;
	std::optional<CrawlerRequest> m_currentRequest;

	QTimer* m_defferedProcessingTimer;
};

}
