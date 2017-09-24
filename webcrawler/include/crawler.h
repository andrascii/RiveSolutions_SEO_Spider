#pragma once

#include "parsed_page.h"
#include "crawler_url_storage.h"
#include "crawler_options.h"

namespace WebCrawler
{

class CrawlerWorkerThread;
class ModelController;
class SequencedDataCollection;
class IQueuedDownloader;

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

	void initCrawlerWorkerThreads();

protected:
	IQueuedDownloader* queuedDownloader() const noexcept;
	virtual IQueuedDownloader* createQueuedDownloader() const noexcept;

private:
	mutable std::unique_ptr<IQueuedDownloader> m_queuedDownloader;

	ModelController* m_modelController;
	
	CrawlerUrlStorage m_urlStorage;

	std::vector<std::unique_ptr<CrawlerWorkerThread>> m_workers;

	QThread* m_crawlerThread;

	unsigned int m_theradCount;
};

}
