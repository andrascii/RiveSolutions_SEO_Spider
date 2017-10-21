#pragma once

#include "crawler.h"
#include "parsed_page.h"
#include "iqueued_dowloader.h"
#include "abstract_threadable_object.h"
#include "page_parsed_data_collector.h"

namespace WebCrawler
{			

class UniqueLinkStore;

class CrawlerWorkerThread : public AbstractThreadableObject
{			
	Q_OBJECT
			
public:
	CrawlerWorkerThread(UniqueLinkStore* crawlerStorage, IQueuedDownloader* queuedDownloader);

	Q_SIGNAL void pageParsed(ParsedPagePtr pageRaw);
	Q_SLOT void applyOptions(const CrawlerOptions& options);

private:
	virtual void process() override;

	void schedulePageResourcesLoading(const ParsedPagePtr& pageRaw) const;

private:
	PageParsedDataCollector* m_pageParsedDataCollector;
	
	UniqueLinkStore* m_uniqueLinkStore;

	IQueuedDownloader* m_queuedDownloader;
};

}
