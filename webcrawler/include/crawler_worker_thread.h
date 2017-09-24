#pragma once

#include "crawler.h"
#include "parsed_page.h"
#include "iqueued_dowloader.h"
#include "abstract_threadable_object.h"
#include "page_parsed_data_collector.h"

namespace WebCrawler
{			

class CrawlerUrlStorage;

class CrawlerWorkerThread : public AbstractThreadableObject
{			
	Q_OBJECT
			
public:
	CrawlerWorkerThread(CrawlerUrlStorage* crawlerStorage, IQueuedDownloader* queuedDownloader);

	Q_SIGNAL void pageParsed(ParsedPagePtr pageRaw);
	Q_SLOT void applyOptions(const CrawlerOptions& options);

private:
	virtual void process() override;

	void schedulePageResourcesLoading(const ParsedPagePtr& pageRaw);

private:
	PageParsedDataCollector* m_pageParsedDataCollector;
	
	CrawlerUrlStorage* m_crawlerInternalUrlStorage;

	IQueuedDownloader* m_queuedDownloader;
};

}
