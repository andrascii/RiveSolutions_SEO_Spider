#pragma once

#include "parsed_page.h"
#include "robots_txt_rules.h"
#include "requester_wrapper.h"
#include "crawler_request.h"

namespace CrawlerEngine
{

struct CrawlerOptions;
struct DownloadResponse;
class IQueuedDownloader;
class UniqueLinkStore;
class OptionsLinkFilter;
class PageDataCollector;
class IUniquenessChecker;

class CrawlerWorkerThread : public QObject
{
	Q_OBJECT

public:
	CrawlerWorkerThread(UniqueLinkStore* uniqueLinkStore);
	std::future<std::vector<CrawlerRequest>> pendingUrls() const;

signals:
	void pageParsed(ParsedPagePtr parsedPage) const;

public slots:
	void startWithOptions(const CrawlerOptions& options, RobotsTxtRules robotsTxtRules);
	void stop();

private slots:
	void extractUrlAndDownload();
	void onCrawlerClearData();

private:
	void schedulePageResourcesLoading(ParsedPagePtr& parsedPage);
	void handlePageLinkList(std::vector<LinkInfo>& linkList, const MetaRobotsFlagsSet& metaRobotsFlags, ParsedPagePtr& parsedPage);
	void onLoadingDone(Requester* requester, const DownloadResponse& response);
	void onStart();
	void onPageParsed(const ParsedPagePtr& parsedPage) const noexcept;

	std::vector<CrawlerRequest> preparePagesAfterStop() const;

private:
	struct PagesAcceptedAfterStop
	{
		using PageRequestPair = std::pair<DownloadRequestType, ParsedPagePtr>;
		using PageRequestPairs = std::vector<PageRequestPair>;

		PageRequestPairs pages;
		mutable std::promise<std::vector<CrawlerRequest>> pagesAcceptedPromise;
	};

	PageDataCollector* m_pageDataCollector;
	UniqueLinkStore* m_uniqueLinkStore;

	std::unique_ptr<OptionsLinkFilter> m_optionsLinkFilter;
	RequesterWrapper m_downloadRequester;

	bool m_isRunning;

	PagesAcceptedAfterStop m_pagesAcceptedAfterStop;
	std::optional<CrawlerRequest> m_currentRequest;

	QTimer* m_defferedProcessingTimer;

	std::unique_ptr<IUniquenessChecker> m_uniquenessChecker;
};

}
