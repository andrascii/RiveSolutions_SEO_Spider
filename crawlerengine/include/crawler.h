#pragma once

#include "parsed_page.h"
#include "unique_link_store.h"
#include "crawler_options.h"
#include "host_info_provider.h"
#include "auto_delete_later_ptr.h"

namespace CrawlerEngine
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
	Crawler(unsigned int threadCount);
	~Crawler();

	SequencedDataCollection* sequencedDataCollection();

signals:
	void crawlingState(CrawlingState state);
	void crawlerStarted();
	void crawlerStopped();

public slots:
	void startCrawling(const CrawlerOptions& options);
	void stopCrawling();

private slots:
	void onAboutCrawlingState();
	void onCrawlingSessionInitialized();

protected:
	IQueuedDownloader* queuedDownloader() const noexcept;
	IRobotsTxtLoader* robotsTxtLoader() const noexcept;

	virtual IQueuedDownloader* createQueuedDownloader() const noexcept;
	virtual IRobotsTxtLoader* createRobotsTxtLoader() const noexcept;
	void createSequencedDataCollection(QThread* targetThread);

	const UniqueLinkStore* crawlerUrlStorage() const noexcept;

private:
	void initCrawlerWorkerThreads();
	bool isPreinitialized() const;
	void initializeCrawlingSession();

private:
	mutable std::unique_ptr<IQueuedDownloader> m_queuedDownloader;
	mutable std::unique_ptr<IRobotsTxtLoader> m_robotsTxtLoader;

	ModelController* m_modelController;
	UniqueLinkStore m_uniqueLinkStore;
	CrawlerOptions m_options;

	unsigned int m_theradCount;
	QTimer* m_crawlingStateTimer;

	std::vector<std::unique_ptr<CrawlerWorkerThread>> m_workers;
	std::unique_ptr<SequencedDataCollection> m_sequencedDataCollection;
};

}
