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

struct CrawlingState
{
	size_t crawledLinkCount;
	size_t pendingLinkCount;
};

Q_DECLARE_METATYPE(CrawlingState)

class Crawler : public QObject
{
	Q_OBJECT

public:
	Crawler(unsigned int threadCount, QThread* sequencedDataCollectionThread = nullptr);
	~Crawler();

	SequencedDataCollection* sequencedDataCollection() const noexcept;
	const CrawlerUrlStorage* crawlerUrlStorage() const noexcept;

signals:
	void crawlingState(CrawlingState state);

public slots:
	void startCrawling(const CrawlerOptions& options);
	void stopCrawling();

private slots:
	void onPageParsed(ParsedPagePtr pageRaw);
	void startCrawlingInternal(const CrawlerOptions& options);
	void stopCrawlingInternal();
	void onAboutCrawlingState();

protected:
	IQueuedDownloader* queuedDownloader() const noexcept;
	virtual IQueuedDownloader* createQueuedDownloader() const noexcept;

private:
	void initCrawlerWorkerThreads();

protected:
	QThread* m_crawlerThread;

private:
	mutable std::unique_ptr<IQueuedDownloader> m_queuedDownloader;

	ModelController* m_modelController;	

	CrawlerUrlStorage m_urlStorage;

	std::vector<std::unique_ptr<CrawlerWorkerThread>> m_workers;

	unsigned int m_theradCount;

	QTimer* m_crawlingStateTimer;
};

}
