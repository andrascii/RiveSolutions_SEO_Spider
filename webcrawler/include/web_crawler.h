#pragma once

#include "page_raw.h"
#include "web_crawler_internal_url_storage.h"
#include "web_crawler_options.h"
#include "queued_downloader.h"

namespace WebCrawler
{

class PageRawProcessor;
class ModelController;

class WebCrawler : public QObject
{
	Q_OBJECT

public:
	WebCrawler(unsigned int threadCount, ModelController* modelController, QObject* parent = nullptr);
	~WebCrawler();

	Q_SLOT void startCrawling(const WebCrawlerOptions& options);
	Q_SLOT void stopCrawling();

private:
	Q_SLOT void onPageRawParsed(PageRawPtr pageRaw);

private:
	QueuedDownloader m_queuedDownloader;

	ModelController* m_modelController;
	
	WebCrawlerInternalUrlStorage m_internalUrlStorage;

	std::vector<std::unique_ptr<PageRawProcessor>> m_workers;
};

}
