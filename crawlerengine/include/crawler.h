#pragma once

#include "parsed_page.h"
#include "unique_link_store.h"
#include "crawler_options.h"

namespace CrawlerEngine
{

struct SiteMapSettings;
class CrawlerWorkerThread;
class ModelController;
class SequencedDataCollection;
class IRobotsTxtLoader;
class IRobotsTxtRules;
class IDownloader;

struct CrawlingProgress
{
	size_t crawledLinkCount;
	size_t pendingLinkCount;
};

Q_DECLARE_METATYPE(CrawlingProgress)

class Crawler : public QObject
{
	Q_OBJECT

public:
	enum State
	{
		StateWorking,
		StatePause,
		StatePending
	};

	static Crawler& instance();

	Crawler(unsigned int threadCount, QObject* parent = nullptr);

	virtual ~Crawler();

	
	void initialize();
	
	void clearData();
	
	State state() const noexcept;
	
	SequencedDataCollection* sequencedDataCollection() const;

	QString siteMapXml(const SiteMapSettings& settings) const;

signals:
	void crawlingProgress(CrawlingProgress state);
	
	void crawlerStarted();
	
	void crawlerStopped();

	void stateChanged(int state);

	void onAboutClearData();

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
	
	void createSequencedDataCollection(QThread* targetThread) const;

	
	const UniqueLinkStore* uniqueLinkStore() const noexcept;

private:
	bool isPreinitialized() const;
	
	void initializeCrawlingSession();

private:
	static Crawler* s_instance;

	mutable std::unique_ptr<IRobotsTxtLoader> m_robotsTxtLoader;

	
	ModelController* m_modelController;
	
	UniqueLinkStore* m_uniqueLinkStore;
	
	CrawlerOptions m_options;

	
	unsigned int m_theradCount;

	
	QTimer* m_crawlingStateTimer;
	
	std::vector<CrawlerWorkerThread*> m_workers;
	
	mutable std::unique_ptr<SequencedDataCollection> m_sequencedDataCollection;
	
	State m_state;
};

}
