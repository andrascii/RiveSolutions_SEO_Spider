#pragma once

#include "parsed_page.h"
#include "robots_txt_rules.h"
#include "requester_wrapper.h"
#include "crawler_request.h"
#include "worker_result.h"
#include "crawler_options.h"

namespace CrawlerEngine
{

class UniqueLinkStore;
class OptionsLinkFilter;
class PageDataCollector;
class ILicenseStateObserver;
class HopsChain;
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

	void reinitOptions(const CrawlerOptionsData& optionsData, RobotsTxtRules robotsTxtRules);

private slots:
	void extractUrlAndDownload();
	void onCrawlerClearData();

private:
	struct ShedulePagesResult
	{
		std::vector<ResourceOnPage> blockedByRobotsTxtLinks;
		std::vector<ResourceOnPage> tooLongLinks;
	};

	void onStart();

	ShedulePagesResult schedulePageResourcesLoading(ParsedPagePtr& parsedPage);

	ShedulePagesResult handlePageLinkList(std::vector<ResourceOnPage>& linkList,
		const MetaRobotsFlagsSet& metaRobotsFlags,
		ParsedPagePtr& parsedPage);

	void onLoadingDone(Requester* requester, const DownloadResponse& response);
	void onPageParsed(const WorkerResult& result) const noexcept;
	void fixDDOSGuardRedirectsIfNeeded(std::vector<ParsedPagePtr>& pages) const;
	std::optional<CrawlerRequest> prepareUnloadedPage() const;
	void handlePage(ParsedPagePtr& page, bool isUrlAdded, DownloadRequestType requestType);
	void handleResponseData(const HopsChain& hopsChain, DownloadRequestType requestType);

private:
	static std::atomic<size_t> s_trialLicenseSentLinksCounter;

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

	PagesAcceptedAfterStop m_pagesAcceptedAfterStop;
	std::optional<CrawlerRequest> m_currentRequest;

	QTimer* m_defferedProcessingTimer;
	ILicenseStateObserver* m_licenseService;
	CrawlerOptionsData m_optionsData;

	std::vector<bool> m_storagesBeforeRemoving;
};

}
