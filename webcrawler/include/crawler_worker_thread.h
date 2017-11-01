#pragma once

#include "crawler.h"
#include "parsed_page.h"
#include "iqueued_dowloader.h"
#include "abstract_threadable_object.h"
#include "page_parsed_data_collector.h"
#include "robots_txt_rules.h"
#include "options_link_filter.h"

namespace WebCrawler
{			

class UniqueLinkStore;

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
	void handlePageLinkList(std::vector<LinkInfo>& linkList, MetaRobotsFlags metaRobotsFlags) const;

private:
	PageParsedDataCollector* m_pageParsedDataCollector;
	UniqueLinkStore* m_uniqueLinkStore;
	IQueuedDownloader* m_queuedDownloader;
	std::unique_ptr<OptionsLinkFilter> m_optionsLinkFilter;
};

}
