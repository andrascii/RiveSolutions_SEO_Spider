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
	CrawlerWorkerThread(UniqueLinkStore* crawlerStorage, IQueuedDownloader* queuedDownloader);

signals:
	void pageParsed(ParsedPagePtr parsedPage) const;

public slots:
	void applyOptions(const CrawlerOptions& options, RobotsTxtRules robotsTxtRules);

	void startWithOptions(const CrawlerOptions& options, RobotsTxtRules robotsTxtRules);

	void stop();

private slots:
	void extractUrlAndDownload();

private:
	void schedulePageResourcesLoading(const ParsedPagePtr& parsedPage) const;

	void handlePageLinkList(std::vector<LinkInfo>& linkList, MetaRobotsFlags metaRobotsFlags) const;

	void onLoadingDone(Common::Requester* requester, const DownloadResponse& response);

private:
	PageDataCollector* m_pageDataCollector;

	UniqueLinkStore* m_uniqueLinkStore;

	IQueuedDownloader* m_queuedDownloader;

	std::unique_ptr<OptionsLinkFilter> m_optionsLinkFilter;

	Common::RequesterWrapper m_downloadRequester;

	bool m_isRunning;
};

}
