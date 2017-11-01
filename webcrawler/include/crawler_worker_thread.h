#pragma once

#include "parsed_page.h"
#include "abstract_threadable_object.h"
#include "robots_txt_rules.h"

namespace WebCrawler
{

class IQueuedDownloader;
class UniqueLinkStore;
class OptionsLinkFilter;
class PageDataCollector;
struct CrawlerOptions;

class CrawlerWorkerThread : public AbstractThreadableObject
{			
	Q_OBJECT
			
public:
	CrawlerWorkerThread(UniqueLinkStore* crawlerStorage, IQueuedDownloader* queuedDownloader);

signals:
	void pageParsed(ParsedPagePtr parsedPage) const;

public slots:
	void applyOptions(const CrawlerOptions& options, RobotsTxtRules robotsTxtRules);

private:
	virtual void process() override;

	void schedulePageResourcesLoading(const ParsedPagePtr& parsedPage) const;
	void handlePageLinkList(std::vector<LinkInfo>& linkList) const;

private:
	PageDataCollector* m_pageDataCollector;
	UniqueLinkStore* m_uniqueLinkStore;
	IQueuedDownloader* m_queuedDownloader;
	std::unique_ptr<OptionsLinkFilter> m_optionsLinkFilter;
};

}
