#pragma once

#include "parsed_page.h"
#include "robots_txt_rules.h"
#include "crawler_request.h"
#include "worker_result.h"
#include "crawler_options.h"
#include "options_link_filter.h"
#include <qobjectdefs.h>

namespace CrawlerEngine
{

class ICrawlerWorkerPageLoader;
class UniqueLinkStore;
class PageDataCollector;
class ILicenseStateObserver;
class HopsChain;
struct DownloadResponse;

class CrawlerWorker : public QObject
{
	Q_OBJECT

public:
	CrawlerWorker(UniqueLinkStore* uniqueLinkStore, ICrawlerWorkerPageLoader* pageLoader);
	std::optional<CrawlerRequest> pendingUrl() const;

signals:
	void workerResult(WorkerResult workerResult) const;

public slots:
	void reinitOptions(const CrawlerOptionsData& optionsData, RobotsTxtRules robotsTxtRules);
	void start(const CrawlerOptionsData& optionsData, RobotsTxtRules robotsTxtRules);
	void stop();

private slots:
	void extractUrlAndDownload();
	void onCrawlerClearData();
	void onLoadingDone(const HopsChain& hopsChain, int requestType);

private:
	struct SchedulePagesResult
	{
		std::vector<ResourceOnPage> blockedByRobotsTxtLinks;
		std::vector<ResourceOnPage> tooLongLinks;
	};

	void onStart();

	SchedulePagesResult schedulePageResourcesLoading(ParsedPagePtr& parsedPage);
	SchedulePagesResult handlePageLinkList(std::vector<ResourceOnPage>& linkList,
		const MetaRobotsFlagsSet& metaRobotsFlags,
		ParsedPagePtr& parsedPage);

	void onPageParsed(const WorkerResult& result) const noexcept;
	void fixDDOSGuardRedirectsIfNeeded(std::vector<ParsedPagePtr>& pages) const;
	void handlePage(ParsedPagePtr& page, bool isUrlAdded, DownloadRequestType requestType);
	void handleResponseData(const HopsChain& hopsChain, DownloadRequestType requestType);

private:
	static std::atomic<size_t> s_trialLicenseSentLinksCounter;

private:
	PageDataCollector* m_pageDataCollector;
	UniqueLinkStore* m_uniqueLinkStore;
	std::unique_ptr<OptionsLinkFilter> m_optionsLinkFilter;

	bool m_isRunning;
	bool m_reloadPage;

	std::optional<CrawlerRequest> m_currentRequest;

	QTimer* m_defferedProcessingTimer;
	ILicenseStateObserver* m_licenseService;
	CrawlerOptionsData m_optionsData;

	std::vector<bool> m_storagesBeforeRemoving;
	ICrawlerWorkerPageLoader* m_pageLoader;
};

}
