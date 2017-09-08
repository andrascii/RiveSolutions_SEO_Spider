#pragma once

#include "page_raw.h"
#include "web_crawler_internal_url_storage.h"
#include "web_crawler_options.h"
#include "queued_downloader.h"

namespace WebCrawler
{

class PageRawProcessor;
class ModelController;
class GuiStorage;

class WebCrawler : public QObject
{
	Q_OBJECT

public:
	WebCrawler(unsigned int threadCount);
	~WebCrawler();

	Q_SLOT void startCrawling(const WebCrawlerOptions& options);
	Q_SLOT void stopCrawling();

	GuiStorage* guiStorage() const noexcept;

private:
	Q_SLOT void onPageRawParsed(PageRawPtr pageRaw);

	Q_SLOT void startCrawlingInternal(const WebCrawlerOptions& options);
	Q_SLOT void stopCrawlingInternal();

private:
	QueuedDownloader m_queuedDownloader;

	ModelController* m_modelController;
	
	WebCrawlerInternalUrlStorage m_internalUrlStorage;

	std::vector<std::unique_ptr<PageRawProcessor>> m_workers;

	QThread* m_crawlerThread;
};

}
