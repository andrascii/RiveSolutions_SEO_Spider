#pragma once

#include "parsed_page.h"
#include "unique_link_store.h"
#include "crawler_options.h"
#include "requester_wrapper.h"
#include "storage_type.h"
#include "web_host_info.h"

namespace CrawlerEngine
{

class ISpecificLoader;
class IRobotsTxtRules;
class IDownloader;
class ITaskProcessor;
class IWebScreenShot;
class IHostInfoProvider;
class CrawlerWorkerThread;
class ModelController;
class SequencedDataCollection;
class Requester;
class HostInfo;
struct SiteMapSettings;
struct TaskResponse;
struct GetHostInfoResponse;

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
		StatePending,
		StateSerializaton,
		StateDeserializaton,
		StatePageRefresh
	};

	static Crawler& instance();

	Crawler(unsigned int threadCount, QObject* parent = nullptr);
	virtual ~Crawler();

	void initialize();
	void clearData();
	bool hasNoData() const noexcept;
	State state() const noexcept;
	SequencedDataCollection* sequencedDataCollection() const;
	QString siteMapXml(const SiteMapSettings& settings) const;
	void saveToFile(const QString& fileName);
	void loadFromFile(const QString& fileName);
	const ISpecificLoader* robotsTxtLoader() const noexcept;
	const ISpecificLoader* xmlSitemapLoader() const noexcept;

	const WebHostInfo* webHostInfo() const;
	std::optional<QByteArray> currentCrawledSiteIPv4() const;
	QString currentCrawledUrl() const noexcept;
	bool readyForRefreshPage() const noexcept;

signals:
	void crawlingProgress(CrawlingProgress state);
	void crawlerStarted();
	void crawlerStopped();
	void crawlerFinished();
	void stateChanged(int state);
	void onAboutClearData();
	void crawlerOptionsChanged(CrawlerOptions options);
	void serializationProcessDone();
	void deserializationProcessDone();
	void refreshPageDone();

public slots:
	void startCrawling(const CrawlerOptions& options);
	void stopCrawling();
	void refreshPage(StorageType storageType, int index);
	void setUserAgent(const QByteArray& userAgent);

protected slots:
	void onRefreshPageDone();

private slots:
	void onAboutCrawlingState();
	void waitSerializationReadyState();
	void onCrawlingSessionInitialized();

protected:
	virtual IHostInfoProvider* createHostInfoProvider() const;
	virtual IWebScreenShot* createWebScreenShot();
	virtual IDownloader* createDownloader() const;
	virtual ITaskProcessor* createTaskProcessor() const;
	virtual void initSequencedDataCollection();
	const UniqueLinkStore* uniqueLinkStore() const noexcept;
	const CrawlerOptions& crawlerOptions() const noexcept;

private:
	bool isPreinitialized() const;
	void initializeCrawlingSession();
	void onSerializationTaskDone(Requester* requester, const TaskResponse& response);
	void onDeserializationTaskDone(Requester* requester, const TaskResponse& response);
	void onHostInfoResponse(Requester* requester, const GetHostInfoResponse& response);

	void onSerializationReadyToBeStarted();
	void onDeserializationReadyToBeStarted();

	void tryToLoadCrawlingDependencies() const;
	void clearDataImpl();

	void setState(State state);

protected:
	std::unique_ptr<SequencedDataCollection> m_sequencedDataCollection;
	ModelController* m_modelController;

private:
	static Crawler* s_instance;

	ISpecificLoader* m_robotsTxtLoader;
	ISpecificLoader* m_xmlSitemapLoader;
	UniqueLinkStore* m_uniqueLinkStore;

	CrawlerOptions m_options;
	unsigned int m_theradCount;

	QTimer* m_crawlingStateTimer;
	QTimer* m_serializatonReadyStateCheckerTimer;

	std::vector<CrawlerWorkerThread*> m_workers;

	State m_state;
	State m_prevState;
	QString m_fileName;

	RequesterWrapper m_serializationRequester;
	RequesterWrapper m_deSerializationRequester;

	IDownloader* m_downloader;

	RequesterWrapper m_hostInfoRequester;
	WebHostInfo* m_webHostInfo;
	std::unique_ptr<HostInfo> m_hostInfo;
};

}
