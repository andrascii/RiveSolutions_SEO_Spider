#pragma once

#include "parsed_page.h"
#include "crawler_url_storage.h"
#include "crawler_options.h"
#include "queued_downloader.h"

namespace WebCrawler
{

class CrawlerWorkerThread;
class ModelController;
class SequencedDataCollection;

class Crawler : public QObject
{
	Q_OBJECT

public:
	Crawler(unsigned int threadCount);
	~Crawler();

	Q_SLOT void startCrawling(const CrawlerOptions& options);
	Q_SLOT void stopCrawling();

	SequencedDataCollection* guiStorage() const noexcept;

	const CrawlerUrlStorage* crawlerUrlStorage() const noexcept;

private:
	Q_SLOT void onPageParsed(ParsedPagePtr pageRaw);

	Q_SLOT void startCrawlingInternal(const CrawlerOptions& options);
	Q_SLOT void stopCrawlingInternal();

private:
	QueuedDownloader m_queuedDownloader;

	ModelController* m_modelController;
	
	CrawlerUrlStorage m_urlStorage;

	std::vector<std::unique_ptr<CrawlerWorkerThread>> m_workers;

	QThread* m_crawlerThread;
};

}
