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

class IWorkerPageLoader;
class UniqueLinkStore;
class PageDataCollector;
class ILicenseStateObserver;
class HopsChain;
struct DownloadResponse;

class CrawlerWorker : public QObject
{
	Q_OBJECT

public:
	CrawlerWorker(UniqueLinkStore* uniqueLinkStore, IWorkerPageLoader* pageLoader);

signals:
	void workerResult(CrawlerEngine::WorkerResult workerResult) const;

public slots:
	void reinitOptions(const CrawlerEngine::CrawlerOptionsData& optionsData, CrawlerEngine::RobotsTxtRules robotsTxtRules);
	void start(const CrawlerEngine::CrawlerOptionsData& optionsData, CrawlerEngine::RobotsTxtRules robotsTxtRules);
	void stop();

private slots:
	void extractUrlAndDownload();
	void onAllLoadedDataToBeCleared();

	void onLoadingDone(const CrawlerEngine::HopsChain& hopsChain,
		int turnaround,
		bool isPageReloaded,
		const std::vector<bool>& reloadingPageStrorages,
		DownloadRequestType requestType);

private:
	struct SchedulePagesResult
	{
		std::vector<ResourceOnPage> blockedByRobotsTxtLinks;
		std::vector<ResourceOnPage> tooLongLinks;
	};

	SchedulePagesResult schedulePageResourcesLoading(ParsedPagePtr& parsedPage);
	SchedulePagesResult handlePageLinkList(std::vector<ResourceOnPage>& linkList,
		const MetaRobotsFlagsSet& metaRobotsFlags,
		ParsedPagePtr& parsedPage);

	void onPageParsed(const WorkerResult& result) const noexcept;
	void fixDDOSGuardRedirectsIfNeeded(std::vector<ParsedPagePtr>& pages) const;

	void handlePage(ParsedPagePtr& page,
		int turnaround,
		bool isUrlAdded,
		bool isPageReloaded,
		const std::vector<bool>& reloadingPageStrorages,
		DownloadRequestType requestType);

	void handleResponseData(const HopsChain& hopsChain,
		int turnaround,
		bool isPageReloaded,
		const std::vector<bool>& reloadingPageStrorages,
		DownloadRequestType requestType);

private:
	PageDataCollector* m_pageDataCollector;
	UniqueLinkStore* m_uniqueLinkStore;
	std::unique_ptr<OptionsLinkFilter> m_optionsLinkFilter;

	bool m_isRunning;

	QTimer* m_defferedProcessingTimer;
	ILicenseStateObserver* m_licenseService;
	CrawlerOptionsData m_optionsData;
	IWorkerPageLoader* m_pageLoader;
};

}
