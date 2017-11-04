#pragma once

#include "parsed_page.h"
#include "unique_link_store.h"
#include "crawler_options.h"

namespace CrawlerEngine
{

class CrawlerWorkerThread;
class ModelController;
class SequencedDataCollection;
class IRobotsTxtLoader;
class IRobotsTxtRules;
class IDownloader;

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
	Crawler(unsigned int threadCount, QObject* parent = nullptr);
	virtual ~Crawler();

	void initialize();

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
	IRobotsTxtLoader* robotsTxtLoader() const;

	virtual IDownloader* createDownloader() const;
	virtual IRobotsTxtLoader* createRobotsTxtLoader() const;
	void createSequencedDataCollection(QThread* targetThread);

	const UniqueLinkStore* uniqueLinkStore() const noexcept;

private:
	bool isPreinitialized() const;
	void initializeCrawlingSession();

private:
	mutable std::unique_ptr<IRobotsTxtLoader> m_robotsTxtLoader;

	ModelController* m_modelController;
	UniqueLinkStore* m_uniqueLinkStore;
	CrawlerOptions m_options;

	unsigned int m_theradCount;
	QTimer* m_crawlingStateTimer;

	std::vector<CrawlerWorkerThread*> m_workers;
	std::unique_ptr<SequencedDataCollection> m_sequencedDataCollection;
};

}
