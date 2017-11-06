#pragma once

#include "parsed_page.h"
#include "robots_txt_rules.h"
#include "requester_wrapper.h"

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

signals:
	void pageParsed(ParsedPagePtr parsedPage) const;

public slots:
	void startWithOptions(const CrawlerOptions& options, RobotsTxtRules robotsTxtRules);

	void stop();

private slots:
	void extractUrlAndDownload();

	void onCrawlerClearData();

private:
	void schedulePageResourcesLoading(const ParsedPagePtr& parsedPage) const;

	void handlePageLinkList(std::vector<LinkInfo>& linkList, const MetaRobotsFlagsSet& metaRobotsFlags) const;

	void onLoadingDone(Requester* requester, const DownloadResponse& response);

	void onStart();

private:
	PageDataCollector* m_pageDataCollector;

	UniqueLinkStore* m_uniqueLinkStore;

	std::unique_ptr<OptionsLinkFilter> m_optionsLinkFilter;

	RequesterWrapper m_downloadRequester;

	bool m_isRunning;

	std::vector<ParsedPagePtr> m_pagesAcceptedAfterStop;
};

}
