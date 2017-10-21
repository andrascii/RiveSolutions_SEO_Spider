#pragma once

#include "parsed_page.h"
#include "unique_link_store.h"
#include "crawler_options.h"

namespace WebCrawler
{

class CrawlerWorkerThread;
class ModelController;
class SequencedDataCollection;
class IQueuedDownloader;
class IRobotsTxtLoader;
class IRobotsTxtRules;

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

signals:
	void crawlingState(CrawlingState state);

public slots:
	void startCrawling(const CrawlerOptions& options);
	void stopCrawling();

private slots:
	void onPageParsed(ParsedPagePtr pageRaw);

	void startCrawlingInternal();
	void stopCrawlingInternal();

	void onAboutCrawlingState();

	void onCrawlingSessionInitialized();

protected:
	IQueuedDownloader* queuedDownloader() const noexcept;
	IRobotsTxtLoader* robotsTxtLoader() const noexcept;
	virtual IQueuedDownloader* createQueuedDownloader() const noexcept;
	virtual IRobotsTxtLoader* createRobotsTxtLoader() const noexcept;
	const UniqueLinkStore* crawlerUrlStorage() const noexcept;

private:
	void initCrawlerWorkerThreads();
	bool isPreinitialized();
	void initializeCrawlingSession();

private:
	mutable std::unique_ptr<IQueuedDownloader> m_queuedDownloader;
	mutable std::unique_ptr<IRobotsTxtLoader> m_robotsTxtLoader;

	ModelController* m_modelController;	
	UniqueLinkStore m_uniqueLinkStore;

	std::vector<std::unique_ptr<CrawlerWorkerThread>> m_workers;

	unsigned int m_theradCount;

	QTimer* m_crawlingStateTimer;
	IRobotsTxtRules* m_robotsTxtRules;

	CrawlerOptions m_options;
};

}
