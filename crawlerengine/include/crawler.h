#pragma once

#include "parsed_page.h"
#include "unique_link_store.h"
#include "crawler_options.h"
#include "requester_wrapper.h"
#include "storage_type.h"
#include "web_host_info.h"
#include "session.h"

namespace CrawlerEngine
{

class ISpecificLoader;
class IRobotsTxtRules;
class IDownloadHandler;
class ITaskProcessor;
class IWebScreenShot;
class IHostInfoProvider;
class CrawlerWorker;
class ModelController;
class SequencedDataCollection;
class Requester;
class HostInfo;
class IScreenshotMaker;
class ICustomDataFeed;
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

		// This state indicates that:
		// 1. crawler has ended crawling or
		// 2. crawling still didn't started
		StatePending,

		// Saving project files with data
		StateSerializaton,

		// Loading project files with data
		StateDeserializaton,

		StatePageRefresh,

		// This state indicates that crawler:
		// 1. checks sitemap
		// 2. checks robots.txt
		// 3. checks IP address using DNS
		// 4. making main page screenshot
		StatePreChecking
	};

	enum DownloaderType
	{
		DownloaderTypeLibCurlMultiSocket,
		DownloaderTypeQNetworkAccessManager
	};

	static Crawler& instance();

	Crawler(unsigned int threadCount, QObject* parent = nullptr);
	virtual ~Crawler();

	void setDownloaderType(DownloaderType type);

	void initialize();
	void clearData();
	bool hasNoData() const noexcept;
	State state() const noexcept;

	SequencedDataCollection* sequencedDataCollection() const;

	QString siteMapXml(const SiteMapSettings& settings) const;

	const ISpecificLoader* robotsTxtLoader() const noexcept;
	const ISpecificLoader* xmlSitemapLoader() const noexcept;
	const WebHostInfo* webHostInfo() const;
	std::optional<QByteArray> currentCrawledSiteIPv4() const;

	QString currentCrawledUrl() const noexcept;
	bool readyForRefreshPage() const noexcept;
	ICrawlerOptions* options() const noexcept;

	Session::State sessionState() const noexcept;
	QString sessionName() const;
	bool hasCustomSessionName() const noexcept;
	bool hasSession() const noexcept;

	size_t scannedPagesCount() const;
	size_t pagesCountOnSite() const;

	void addCustomDataFeed(ICustomDataFeed* customDataFeed);
	ICustomDataFeed* customDataFeedByName(const QString& dataFeedName) const;
	QList<ICustomDataFeed*> customDataFeeds() const;

signals:
	void crawlingProgress(CrawlingProgress progress);
	void crawlerStarted();
	void crawlerStopped();
	void crawlerFinished();
	void stateChanged(int state);
	void onAboutClearData();
	void crawlerOptionsLoaded();
	void serializationProcessDone();
	void deserializationProcessDone();
	void refreshPageDone();
	void sessionCreated();
	void sessionDestroyed();
	void sessionStateChanged(Session::State state);
	void sessionNameChanged(const QString& name);

public slots:
	void startCrawling();
	void stopCrawling();
	void refreshPage(StorageType storageType, int index);
	void setUserAgent(const QByteArray& userAgent);
	void saveFile();
	void closeFile();
	void saveToFile(const QString& fileName);
	void loadFromFile(const QString& fileName);

protected slots:
	void onRefreshPageDone();

private slots:
	void onAboutCrawlingState();
	void waitSerializationReadyState();
	void onCrawlingSessionInitialized();
	void onSessionChanged();
	void onCrawlerOptionsSomethingChanged();
	void onSequencedDataCollectionChanged();
	void onDeserializationProcessDone();

protected:
	virtual IHostInfoProvider* createHostInfoProvider() const;
	virtual IScreenshotMaker* createScreenshotMaker();
	virtual IDownloadHandler* createDownloader() const;
	virtual ITaskProcessor* createTaskProcessor() const;
	virtual void initSequencedDataCollection();
	const UniqueLinkStore* uniqueLinkStore() const noexcept;

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
	void initSessionIfNeeded(Session::State state = Session::StateNone, const QString& name = QString());
	void setCustomDataFeedsToSequencedDataCollection();

protected:
	std::unique_ptr<SequencedDataCollection> m_sequencedDataCollection;
	ModelController* m_modelController;

private:
	static Crawler* s_instance;

	ISpecificLoader* m_robotsTxtLoader;
	ISpecificLoader* m_xmlSitemapLoader;
	UniqueLinkStore* m_uniqueLinkStore;

	ICrawlerOptions* m_options;
	unsigned int m_theradCount;

	QTimer* m_crawlingStateTimer;
	QTimer* m_serializatonReadyStateCheckerTimer;

	std::vector<CrawlerWorker*> m_workers;

	State m_state;
	State m_prevState;

	RequesterWrapper m_serializationRequester;
	RequesterWrapper m_deSerializationRequester;

	IDownloadHandler* m_downloader;

	RequesterWrapper m_hostInfoRequester;
	WebHostInfo* m_webHostInfo;
	std::unique_ptr<HostInfo> m_hostInfo;

	QPointer<Session> m_session;
	ILicenseStateObserver* m_licenseStateObserver;

	QMap<QString, ICustomDataFeed*> m_customDataFeeds;
	DownloaderType m_downloaderType;
};

}
