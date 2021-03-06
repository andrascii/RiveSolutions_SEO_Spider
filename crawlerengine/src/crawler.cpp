#include "crawler.h"
#include "unordered_data_collection.h"
#include "sequenced_data_collection.h"
#include "model_controller.h"
#include "crawler_worker.h"
#include "robots_txt_rules.h"
#include "robots_txt_loader.h"
#include "thread_manager.h"
#include "qt_based_download_handler.h"
#include "host_info_provider.h"
#include "site_map.h"
#include "serializer.h"
#include "task_processor.h"
#include "serialization_tasks.h"
#include "service_locator.h"
#include "inotification_service.h"
#include "notification_service.h"
#include "xml_sitemap_loader.h"
#include "get_host_info_request.h"
#include "get_host_info_response.h"
#include "host_info.h"
#include "isequenced_storage.h"
#include "helpers.h"
#include "license_handler.h"
#include "license_state_observer.h"
#include "common_constants.h"
#include "proper_404_checker.h"
#ifdef ENABLE_SCREENSHOTS
#include "screenshot_maker.h"
#endif
#include "icustom_data_feed.h"
#include "multi_socket_download_handler.h"
#include "multi_request_page_loader.h"
#include "qt_page_loader.h"

namespace CrawlerEngine
{

Crawler* Crawler::s_instance = nullptr;

Crawler& Crawler::instance()
{
	return *s_instance;
}

Crawler::Crawler(QObject* parent)
	: QObject(parent)
    , m_sequencedDataCollection(nullptr)
    , m_modelController(nullptr)
    , m_robotsTxtLoader(new RobotsTxtLoader(this))
    , m_xmlSitemapLoader(new XmlSitemapLoader(static_cast<RobotsTxtLoader*>(m_robotsTxtLoader), this))
    , m_uniqueLinkStore(nullptr)
    , m_options(new CrawlerOptions(this))
    , m_theradCount(0)
    , m_crawlingStateTimer(new QTimer(this))
	, m_serializatonReadyStateCheckerTimer(new QTimer(this))
	, m_state(StatePending)
	, m_downloader(nullptr)
	, m_webHostInfo(nullptr)
	, m_licenseStateObserver(nullptr)
	, m_downloaderType(DownloaderTypeQNetworkAccessManager)
	, m_crawlingFinished(false)
{
	ServiceLocator* serviceLocator = ServiceLocator::instance();
	serviceLocator->addService<INotificationService>(new NotificationService);

	ASSERT(s_instance == nullptr && "Allowed only one instance of Crawler");
	ASSERT(qRegisterMetaType<WorkerResult>());
	ASSERT(qRegisterMetaType<ParsedPagePtr>());
	ASSERT(qRegisterMetaType<std::vector<ParsedPagePtr>>());
	ASSERT(qRegisterMetaType<CrawlingProgress>());
	ASSERT(qRegisterMetaType<CrawlerOptionsData>() > -1);
	ASSERT(qRegisterMetaType<RobotsTxtRules>());
	ASSERT(qRegisterMetaType<ICustomDataFeedRow*>());

	VERIFY(connect(m_crawlingStateTimer, &QTimer::timeout, this, &Crawler::onAboutCrawlingState));
	VERIFY(connect(m_serializatonReadyStateCheckerTimer, &QTimer::timeout, this, &Crawler::waitSerializationReadyState));
	VERIFY(connect(this, &Crawler::deserializationProcessDone, this, &Crawler::onDeserializationProcessDone));

	Common::Helpers::connectSignalsToMetaMethod(
		options()->qobject(),
		Common::Helpers::allUserSignals(options()->qobject()),
		this,
		Common::Helpers::metaMethodOfSlot(this, "onCrawlerOptionsSomethingChanged()")
	);

	VERIFY(connect(m_robotsTxtLoader->qobject(), SIGNAL(ready()), this, SLOT(onCrawlingSessionInitialized()), Qt::QueuedConnection));
	VERIFY(connect(m_xmlSitemapLoader->qobject(), SIGNAL(ready()), this, SLOT(onCrawlingSessionInitialized()), Qt::QueuedConnection));

	m_crawlingStateTimer->setInterval(100);
	m_serializatonReadyStateCheckerTimer->setInterval(200);

	s_instance = this;
}

Crawler::~Crawler()
{
	for (CrawlerWorker* worker : m_workers)
	{
		VERIFY(QMetaObject::invokeMethod(worker, "stop", Qt::BlockingQueuedConnection));
	}

	ThreadManager::destroy();

	s_instance = nullptr;

	ServiceLocator::instance()->destroyService<INotificationService>();
	ServiceLocator::instance()->destroyService<ILicenseStateObserver>();

	qDeleteAll(m_customDataFeeds);
}

void Crawler::setDownloaderType(DownloaderType type)
{
	ASSERT(type == DownloaderTypeLibCurlMultiSocket || type == DownloaderTypeQNetworkAccessManager);
	m_downloaderType = type;
}

void Crawler::initialize()
{
	m_modelController = new ModelController;

	initSequencedDataCollection();

	m_downloader = createDownloader();
	m_webHostInfo = new WebHostInfo(this, m_xmlSitemapLoader, m_robotsTxtLoader);

#ifdef ENABLE_SCREENSHOTS
	VERIFY(connect(m_webHostInfo, &WebHostInfo::webScreenshotLoaded, this, &Crawler::onSessionChanged));
#endif

	ThreadManager& threadManager = ThreadManager::instance();

	threadManager.moveObjectToThread(m_downloader->qobject(), "DownloaderThread");
	threadManager.moveObjectToThread(m_modelController, "BackgroundThread");
	threadManager.moveObjectToThread(createHostInfoProvider()->qobject(), "BackgroundThread");
	threadManager.moveObjectToThread(createTaskProcessor()->qobject(), "SerializerThread");
	threadManager.moveObjectToThread(new Proper404Checker, "BackgroundThread");
	threadManager.moveObjectToThread(new LicenseHandler, "BackgroundThread");
#ifdef ENABLE_SCREENSHOTS
	threadManager.moveObjectToThread(createScreenshotMaker()->qobject(), "BackgroundThread");
#endif

	m_licenseStateObserver = new LicenseStateObserver;

	ServiceLocator::instance()->addService<ILicenseStateObserver>(m_licenseStateObserver);

	m_uniqueLinkStore = new UniqueLinkStore(this);

	for (unsigned i = 0; i < workerCount(); ++i)
	{
		m_workers.push_back(new CrawlerWorker(m_uniqueLinkStore, createWorkerPageLoader()));

		VERIFY(connect(m_workers.back(), SIGNAL(workerResult(CrawlerEngine::WorkerResult)),
			m_modelController, SLOT(handleWorkerResult(CrawlerEngine::WorkerResult)), Qt::QueuedConnection));

		threadManager.moveObjectToThread(m_workers.back(), QString("CrawlerWorkerThread#%1").arg(i).toLatin1());
	}

	VERIFY(connect(m_modelController, &ModelController::refreshPageDone, this, &Crawler::onRefreshPageDone, Qt::QueuedConnection));
}

void Crawler::clearData()
{
	ASSERT(state() == StatePending || state() == StatePause);

	clearDataImpl();

	setState(StatePending);

	m_hostInfo.reset();

	emit onAboutClearData();
}

void Crawler::clearDataImpl()
{
	m_crawlingFinished = false;
	CrawlerSharedState::instance()->clear();
	VERIFY(QMetaObject::invokeMethod(m_modelController, "clearData", Qt::BlockingQueuedConnection));
	m_uniqueLinkStore->clear();
	if (m_session != nullptr && m_session->hasCustomName())
	{
		m_session->setState(Session::StateUnsaved);
	}
	else
	{
		m_session->deleteLater();
	}
}

void Crawler::setState(State state)
{
	m_prevState = m_state;
	m_state = state;
	emit stateChanged(state);
}

void Crawler::initSessionIfNeeded(Session::State state, const QString& name)
{
	if (m_session)
	{
		return;
	}

	m_session = new Session(state, name, this);

	VERIFY(connect(m_session, &Session::stateChanged, this, &Crawler::sessionStateChanged));
	VERIFY(connect(m_session, &Session::nameChanged, this, &Crawler::sessionNameChanged));
	VERIFY(connect(m_session, &Session::destroyed, this, &Crawler::sessionDestroyed));

	emit sessionCreated();
}

void Crawler::setCustomDataFeedsToSequencedDataCollection()
{
	ASSERT(m_sequencedDataCollection);
	m_sequencedDataCollection->setCustomDataFeeds(m_customDataFeeds.values().toVector());
}

unsigned Crawler::workerCount() const noexcept
{
	const unsigned hardwareConcurrency = std::thread::hardware_concurrency();

	if (m_theradCount == 0 || m_theradCount > hardwareConcurrency * 2)
	{

#ifdef QT_DEBUG
		return hardwareConcurrency / 2;
#else
		return hardwareConcurrency;
#endif

	}

	return m_theradCount;
}

bool Crawler::hasNoData() const noexcept
{
	return m_sequencedDataCollection->empty();
}

Crawler::State Crawler::state() const noexcept
{
	return m_state;
}

void Crawler::startCrawling()
{
	m_crawlingFinished = false;
	setState(StatePreChecking);

	if (m_options->pauseRangeFrom() == -1 && m_options->pauseRangeTo() == -1 ||
		m_options->pauseRangeFrom() == 0 && m_options->pauseRangeTo() == 0 || !m_options->pauseRangeEnabled())
	{
		VERIFY(QMetaObject::invokeMethod(m_downloader->qobject(), "resetPauseRange", Qt::BlockingQueuedConnection));
	}
	else
	{
		const int rangeFrom = qMax(0, m_options->pauseRangeFrom());
		const int rangeTo = qMax(rangeFrom, m_options->pauseRangeTo());
		VERIFY(QMetaObject::invokeMethod(m_downloader->qobject(), "setPauseRange",
			Qt::BlockingQueuedConnection, Q_ARG(int, rangeFrom), Q_ARG(int, rangeTo)));
	}

	VERIFY(QMetaObject::invokeMethod(m_downloader->qobject(), "setTimeout",
		Qt::BlockingQueuedConnection, Q_ARG(int, m_options->limitTimeout())));

	VERIFY(QMetaObject::invokeMethod(m_downloader->qobject(), "setMaxRedirects",
		Qt::BlockingQueuedConnection, Q_ARG(int, m_options->maxRedirectsToFollow())));

	VERIFY(QMetaObject::invokeMethod(m_downloader->qobject(), "setMaxParallelConnections",
		Qt::BlockingQueuedConnection, Q_ARG(int, m_options->maxParallelConnections())));

	m_uniqueLinkStore->setLimitCrawledLinksCount(m_options->limitSearchTotal());

	initializeCrawlingSession();
}

void Crawler::stopCrawling()
{
	if (state() == StatePause || state() == StatePending)
	{
		return;
	}

	setState(StatePause);

	for (CrawlerWorker* worker : m_workers)
	{
		VERIFY(QMetaObject::invokeMethod(worker, "stop", Qt::BlockingQueuedConnection));
	}

	m_crawlingStateTimer->stop();

	emit crawlerStopped();

	ServiceLocator* serviceLocator = ServiceLocator::instance();
	serviceLocator->service<INotificationService>()->info(tr("Crawler state"), tr("Crawler stopped"));
}

void Crawler::onAboutCrawlingState()
{
	CrawlingProgress progress;

	const CrawlerSharedState* state = CrawlerSharedState::instance();

	const int sequencedDataCollectionCount = state->sequencedDataCollectionLinksCount();
	const int modelControllerCrawledLinksCount = state->modelControllerCrawledLinksCount();
	const int modelControllerAcceptedLinksCount = state->modelControllerAcceptedLinksCount();
	const int uniqueLinkStoreCrawledCount = state->downloaderCrawledLinksCount();
	const int uniqueLinkStorePendingCount = state->downloaderPendingLinksCount();

	const size_t controllerPending = qMax(uniqueLinkStoreCrawledCount, modelControllerCrawledLinksCount) - modelControllerCrawledLinksCount;
	const size_t seqCollPending = qMax(modelControllerAcceptedLinksCount, sequencedDataCollectionCount) - sequencedDataCollectionCount;
	const size_t additionalPendingCount = controllerPending + seqCollPending;

	progress.crawledLinkCount = sequencedDataCollectionCount;
	progress.pendingLinkCount = uniqueLinkStorePendingCount + additionalPendingCount;

	emit crawlingProgress(progress);

	const bool isCrawlingEnded = uniqueLinkStorePendingCount == 0 &&
		sequencedDataCollectionCount > 0 &&
		uniqueLinkStoreCrawledCount == state->workersProcessedLinksCount() &&
		state->modelControllerCrawledLinksCount() == state->workersProcessedLinksCount() &&
		modelControllerAcceptedLinksCount == sequencedDataCollectionCount;

	const bool isTrialLicenseMaxLinks = m_licenseStateObserver->isTrialLicense() &&
		sequencedDataCollectionCount >= Common::c_maxTrialLicenseCrawlingLinksCount;

	if (isTrialLicenseMaxLinks || isCrawlingEnded)
	{
		stopCrawling();
		setState(StatePending);

		ServiceLocator* serviceLocator = ServiceLocator::instance();
		serviceLocator->service<INotificationService>()->info(tr("Crawler state"), tr("Program has ended crawling"));
		m_crawlingFinished = true;

		emit crawlerFinished();
	}
}

void Crawler::waitSerializationReadyState()
{
	const CrawlerSharedState* crawlerSharedState = CrawlerSharedState::instance();

	ASSERT(m_session);

	const bool isReadyForSerialization =
		(state() == StateSerializaton || state() == StateDeserializaton) && !m_session->name().isEmpty() &&
		crawlerSharedState->workersProcessedLinksCount() == crawlerSharedState->modelControllerCrawledLinksCount() &&
		crawlerSharedState->modelControllerAcceptedLinksCount() == crawlerSharedState->sequencedDataCollectionLinksCount();

	if (!isReadyForSerialization)
	{
		return;
	}

	if (state() == StateSerializaton)
	{
		onSerializationReadyToBeStarted();
	}
	else if (state() == StateDeserializaton)
	{
		onDeserializationReadyToBeStarted();
	}

	m_serializatonReadyStateCheckerTimer->stop();
}

void Crawler::onCrawlingSessionInitialized()
{
	if (!isPreinitialized())
	{
		return;
	}

	setState(StateWorking);

	initSessionIfNeeded();

	VERIFY(QMetaObject::invokeMethod(m_modelController, "setWebCrawlerOptions",
		Qt::BlockingQueuedConnection, Q_ARG(const CrawlerEngine::CrawlerOptionsData&, m_options->data())));

	setUserAgent(m_options->userAgent());

	if (m_options->useProxy())
	{
		VERIFY(QMetaObject::invokeMethod(m_downloader->qobject(), "setProxy",
			Qt::BlockingQueuedConnection,
			Q_ARG(const QString&, m_options->proxyHostName()),
			Q_ARG(int, m_options->proxyPort()),
			Q_ARG(const QString&, m_options->proxyUser()),
			Q_ARG(const QString&, m_options->proxyPassword())));
	}
	else
	{
		VERIFY(QMetaObject::invokeMethod(m_downloader->qobject(), "resetProxy", Qt::BlockingQueuedConnection));
	}

	m_uniqueLinkStore->addUrl(m_options->startCrawlingPage(), DownloadRequestType::RequestTypeGet);

	for (CrawlerWorker* worker : m_workers)
	{
		VERIFY(QMetaObject::invokeMethod(worker, "start", Qt::QueuedConnection,
			Q_ARG(const CrawlerEngine::CrawlerOptionsData&, m_options->data()),
            Q_ARG(CrawlerEngine::RobotsTxtRules, RobotsTxtRules(m_robotsTxtLoader->content()))));
	}

	m_crawlingStateTimer->start();

	emit crawlerStarted();

	ServiceLocator::instance()->service<INotificationService>()->info(tr("Crawler state"), tr("Crawler started"));
}

void Crawler::onSessionChanged()
{
	if (!m_session || state() == StateDeserializaton)
	{
		return;
	}

	m_session->setState(Session::StateUnsaved);
}

void Crawler::onCrawlerOptionsSomethingChanged()
{
	ASSERT(state() == StatePending ||
		state() == StateDeserializaton ||
		state() == StatePreChecking
	);

	onSessionChanged();
}

void Crawler::onSequencedDataCollectionChanged()
{
	if (state() != StateWorking)
	{
		return;
	}

	onSessionChanged();
}

void Crawler::onDeserializationProcessDone()
{
	for (auto worker : m_workers)
	{
		VERIFY(QMetaObject::invokeMethod(worker, "reinitOptions", Qt::BlockingQueuedConnection,
			Q_ARG(const CrawlerEngine::CrawlerOptionsData&, m_options->data()),
			Q_ARG(CrawlerEngine::RobotsTxtRules, RobotsTxtRules(m_robotsTxtLoader->content())))
		);
	}
}

void Crawler::onRefreshPageDone()
{
	ServiceLocator::instance()->service<INotificationService>()->info(
		tr("Refreshing page"),
		tr("Page refresh completed.")
	);

	setState(m_prevState);

	emit refreshPageDone();
}

bool Crawler::isPreinitialized() const
{
	return m_robotsTxtLoader->isReady() && m_xmlSitemapLoader->isReady();
}

void Crawler::initializeCrawlingSession()
{
	if (m_hostInfo)
	{
		tryToLoadCrawlingDependencies();
		return;
	}

	GetHostInfoRequest request(m_options->startCrawlingPage());

	m_hostInfoRequester.reset(request, this, &Crawler::onHostInfoResponse);

	m_hostInfoRequester->start();
}

void Crawler::onSerializationTaskDone(Requester* requester, const TaskResponse& response)
{
	Q_UNUSED(requester);

	SerializationTaskResponseResult* result = Common::Helpers::fast_cast<SerializationTaskResponseResult*>(response.result.get());

	ASSERT(result);

	m_state = m_prevState;

	emit stateChanged(m_state);

	if (!result->error.isEmpty())
	{
		ServiceLocator* serviceLocator = ServiceLocator::instance();

		ASSERT(serviceLocator->isRegistered<INotificationService>());

		serviceLocator->service<INotificationService>()->error(
			tr("Save file error"),
			tr("The operation has not been successful.")
		);
	}

	m_serializationRequester.reset();

	ASSERT(m_session);

	m_session->setState(Session::StateSaved);

	emit serializationProcessDone();
}

void Crawler::onDeserializationTaskDone(Requester* requester, const TaskResponse& response)
{
	Q_UNUSED(requester);

	SerializationTaskResponseResult* result =
		Common::Helpers::fast_cast<SerializationTaskResponseResult*>(response.result.get());

	const int turnaround = CrawlerSharedState::instance()->turnaround();

	ASSERT(result);

	if (!result->error.isEmpty())
	{
		ServiceLocator* serviceLocator = ServiceLocator::instance();
		ASSERT(serviceLocator->isRegistered<INotificationService>());

		serviceLocator->service<INotificationService>()->error(tr("Loading file error"),
			tr("The file could not be opened. If you're openning the file from "
			"the recent files menu you should make sure that this file still exists"));

		ASSERT(m_session);
		m_session->deleteLater();

		TaskRequest* taskRequest = Common::Helpers::fast_cast<TaskRequest*>(requester->request());
		emit deserializationFailed(taskRequest->task->fileName());
	}
	else
	{
		const std::vector<ParsedPagePtr>& pages = result->serializer->pages();

		int crawledLinksCount = 0;

		for (const ParsedPagePtr& page : pages)
		{
			const std::vector<bool> storages(std::move(page->storages));
			page->storages = std::vector<bool>(storages.size(), false);
			for (size_t i = 0; i < storages.size(); ++i)
			{
				if (storages[i])
				{
					const StorageType storage = static_cast<StorageType>(i);

					if (storage == StorageType::CrawledUrlStorageType)
					{
						++crawledLinksCount;
					}

					m_modelController->data()->addParsedPage(page, static_cast<int>(i), turnaround);
				}
			}
		}

		m_uniqueLinkStore->setCrawledUrls(result->serializer->crawledLinks());
		m_uniqueLinkStore->setPendingUrls(result->serializer->pendingLinks());

		m_options->setData(result->serializer->crawlerOptionsData());
		m_webHostInfo->setData(result->serializer->webHostInfoData());
		emit crawlerOptionsLoaded();

		CrawlerSharedState* state = CrawlerSharedState::instance();
		state->setDownloaderCrawledLinksCount(static_cast<int>(result->serializer->crawledLinks().size()));
		state->setDownloaderPendingLinksCount(static_cast<int>(result->serializer->pendingLinks().size()));
		state->setWorkersProcessedLinksCount(crawledLinksCount);
		state->setModelControllerAcceptedLinksCount(crawledLinksCount);
		state->setModelControllerCrawledLinksCount(crawledLinksCount);

		emit deserializationProcessDone();
	}

	m_state = m_prevState;
	emit stateChanged(m_state);

	m_deSerializationRequester.reset();
}

void Crawler::onHostInfoResponse(Requester*, const GetHostInfoResponse& response)
{
	m_hostInfoRequester->stop();

	if (!response.hostInfo.isValid())
	{
		ServiceLocator* serviceLocator = ServiceLocator::instance();

		serviceLocator->service<INotificationService>()->error(
			tr("DNS Lookup Failed!"),
			tr("I'm sorry but I cannot find this website\n"
				"Please, be sure that you entered a valid address")
		);

		setState(StatePending);
		emit crawlerFailed();

		return;
	}

	m_hostInfo.reset(new HostInfo(response.hostInfo));
	m_options->setStartCrawlingPage(response.url);

	tryToLoadCrawlingDependencies();
}

void Crawler::onSerializationReadyToBeStarted()
{
	ASSERT(state() == StateSerializaton);

	const SequencedDataCollection* sequencedCollection = sequencedDataCollection();

	const ISequencedStorage* storage = sequencedCollection->storage(StorageType::CrawledUrlStorageType);
	std::vector<ParsedPagePtr> pendingPages = m_modelController->data()->allParsedPages(StorageType::PendingResourcesStorageType);
	std::vector<ParsedPagePtr> nonLoadingPages = m_modelController->data()->allParsedPages(StorageType::AllOtherResourcesStorageType);

	const int pagesCount = storage->size() + static_cast<int>(pendingPages.size());

	std::vector<ParsedPage*> pages;
	pages.reserve(pagesCount);

	for (int i = 0; i < storage->size(); ++i)
	{
		const ParsedPage* page = (*storage)[i];
		pages.push_back(const_cast<ParsedPage*>(page));
	}

	for (size_t i = 0; i < pendingPages.size(); ++i)
	{
		ParsedPage* page = pendingPages[i].get();
		pages.push_back(page);
	}

	for (size_t i = 0; i < nonLoadingPages.size(); ++i)
	{
		ParsedPage* page = nonLoadingPages[i].get();
		pages.push_back(page);
	}

	std::vector<CrawlerRequest> pendingUrls;

	std::vector<CrawlerRequest> linkStorePendingUrls = m_uniqueLinkStore->pendingAndActiveUrls();
	pendingUrls.insert(pendingUrls.end(), linkStorePendingUrls.begin(), linkStorePendingUrls.end());

	std::vector<CrawlerRequest> crawledUrls = m_uniqueLinkStore->crawledUrls();

	std::unique_ptr<Serializer> serializer = std::make_unique<Serializer>(std::move(pages),
		std::move(crawledUrls), std::move(pendingUrls), m_options->data(), m_webHostInfo->allData());

	std::shared_ptr<ITask> task = std::make_shared<SerializationTask>(std::move(serializer), m_session->name());

	TaskRequest request(task);
	m_serializationRequester.reset(request, this, &Crawler::onSerializationTaskDone);
	m_serializationRequester->start();
}

void Crawler::onDeserializationReadyToBeStarted()
{
	ASSERT(state() == StateDeserializaton);

	clearDataImpl();

	std::unique_ptr<Serializer> serializer = std::make_unique<Serializer>();
	std::shared_ptr<ITask> task = std::make_shared<DeserializatoinTask>(std::move(serializer), m_session->name());

	TaskRequest request(task);
	m_deSerializationRequester.reset(request, this, &Crawler::onDeserializationTaskDone);
	m_deSerializationRequester->start();
}

void Crawler::tryToLoadCrawlingDependencies()
{
	DEBUG_ASSERT(m_options->startCrawlingPage().isValid());

	if (isPreinitialized())
	{
		onCrawlingSessionInitialized();
		return;
	}

	m_robotsTxtLoader->setHost(m_options->startCrawlingPage());
	m_xmlSitemapLoader->setHost(m_options->startCrawlingPage());
	m_webHostInfo->reset(m_options->startCrawlingPage());
	m_robotsTxtLoader->load();
	m_xmlSitemapLoader->load();
}

void Crawler::initSequencedDataCollection()
{
	m_sequencedDataCollection = std::make_unique<SequencedDataCollection>(m_modelController->data());

	m_sequencedDataCollection->initialize();

	Common::Helpers::connectSignalsToMetaMethod(
		sequencedDataCollection(),
		Common::Helpers::allUserSignals(sequencedDataCollection(), QVector<QByteArray>() << "beginClearData()"),
		this,
		Common::Helpers::metaMethodOfSlot(this, "onSequencedDataCollectionChanged()")
	);

	// jus for test
	// addCustomDataFeed(new TestDataFeed());

	// addCustomDataFeed(new YandexMetricaDataFeed());

	setCustomDataFeedsToSequencedDataCollection();
}

IWorkerPageLoader* Crawler::createWorkerPageLoader() const
{
	switch (m_downloaderType)
	{
		case DownloaderTypeQNetworkAccessManager:
		{
			INFOLOG << "Creating QtPageLoader";
			return new QtPageLoader(m_uniqueLinkStore);
		}
		case DownloaderTypeLibCurlMultiSocket:
		{
			INFOLOG << "Creating MultiRequestPageLoader";
			return new MultiRequestPageLoader(m_uniqueLinkStore);
		}
	}

	ASSERT(!"Invalid downloader type");

	return nullptr;
}

IHostInfoProvider* Crawler::createHostInfoProvider() const
{
	return new HostInfoProvider;
}

#ifdef ENABLE_SCREENSHOTS
IScreenshotMaker* Crawler::createScreenshotMaker()
{
    return new ScreenshotMaker;
}
#endif

IDownloadHandler* Crawler::createDownloader() const
{
	switch (m_downloaderType)
	{
		case DownloaderTypeQNetworkAccessManager:
		{
			INFOLOG << "Creating QtBasedDownloadHandler";
			return new QtBasedDownloadHandler;
		}
		case DownloaderTypeLibCurlMultiSocket:
		{
			INFOLOG << "Creating MultiSocketDownloadHandler";
			return new MultiSocketDownloadHandler;
		}
	}

	ASSERT(!"Invalid downloader type");

	return nullptr;
}

ITaskProcessor* Crawler::createTaskProcessor() const
{
	ITaskProcessor* processor = new TaskProcessor();
	return processor;
}

SequencedDataCollection* Crawler::sequencedDataCollection() const
{
	return m_sequencedDataCollection.get();
}

QString Crawler::siteMapXml(const SiteMapSettings& settings) const
{
	SiteMap siteMap;

	const SequencedDataCollection* sequencedCollection = sequencedDataCollection();
	const ISequencedStorage* storage = sequencedCollection->storage(StorageType::CrawledUrlStorageType);

	return siteMap.xml(*storage, settings);
}

void Crawler::saveFile()
{
	ASSERT(QThread::currentThread() == thread());
	ASSERT(state() != State::StateWorking);

	setState(StateSerializaton);

	m_serializatonReadyStateCheckerTimer->start();
	waitSerializationReadyState();
}

void Crawler::closeFile()
{
	ASSERT(QThread::currentThread() == thread());

	if (!m_session)
	{
		return;
	}

	if (state() == StateWorking)
	{
		ServiceLocator::instance()->service<INotificationService>()->warning(
			tr("Closing file error"),
			tr("Can't close project file while crawler is working!")
		);

		return;
	}

	clearData();

	m_session->deleteLater();
}

void Crawler::saveToFile(const QString& fileName)
{
	ASSERT(m_session);

	m_session->setName(fileName);

	saveFile();
}

void Crawler::loadFromFile(const QString& fileName)
{
	ASSERT(QThread::currentThread() == thread());

	ServiceLocator* serviceLocator = ServiceLocator::instance();

	if (m_session)
	{
		serviceLocator->service<INotificationService>()->error(
			tr("Error"),
			tr("Cannot load project file until the existing one is saved")
		);

		return;
	}

	if (state() == Crawler::StateWorking)
	{
		serviceLocator->service<INotificationService>()->error(
			tr("Error"),
			tr("Cannot open a document while crawler is working!")
		);

		return;
	}

	initSessionIfNeeded(Session::StateSaved, fileName);

	setState(StateDeserializaton);

	m_serializatonReadyStateCheckerTimer->start();

	waitSerializationReadyState();
}

const ISpecificLoader* Crawler::robotsTxtLoader() const noexcept
{
	return m_robotsTxtLoader;
}

const ISpecificLoader* Crawler::xmlSitemapLoader() const noexcept
{
	return m_xmlSitemapLoader;
}

const WebHostInfo * Crawler::webHostInfo() const
{
	return m_webHostInfo;
}

std::optional<QByteArray> Crawler::currentCrawledSiteIPv4() const
{
	if (m_hostInfo)
	{
		QList<QByteArray> ipv4Addresses = m_hostInfo->stringAddressesIPv4();

		if (!ipv4Addresses.isEmpty())
		{
			return ipv4Addresses.front();
		}
	}

	return std::make_optional<QByteArray>();
}

QString Crawler::currentCrawledUrl() const noexcept
{
	return m_options->startCrawlingPage().urlStr();
}

void Crawler::refreshPage(StorageType storageType, int index)
{
	INFOLOG << "Refresh page. index = " << index << "; storageType = " << static_cast<int>(storageType) << ";";

	if (!readyForRefreshPage())
	{
		WARNLOG << "Crawler is not ready for refresh pages";
		return;
	}

	ASSERT(!m_workers.empty());

	ParsedPage* parsedPage = m_sequencedDataCollection->storage(storageType)->get(index);
	std::vector<bool> storagesBeforeRemoving = parsedPage->storages;

	ASSERT(parsedPage->canRefresh());

	INFOLOG << "Target storage size = " << m_sequencedDataCollection->storage(storageType)->size();

	VERIFY(QMetaObject::invokeMethod(m_modelController, "preparePageForRefresh",
		Qt::BlockingQueuedConnection, Q_ARG(CrawlerEngine::ParsedPage*, parsedPage),
        Q_ARG(int, CrawlerEngine::CrawlerSharedState::instance()->turnaround())));

	m_uniqueLinkStore->addRefreshUrl(parsedPage->url, DownloadRequestType::RequestTypeGet, storagesBeforeRemoving);

	setState(StatePageRefresh);
}

void Crawler::setUserAgent(const QByteArray& userAgent)
{
	VERIFY(QMetaObject::invokeMethod(m_downloader->qobject(), "setUserAgent",
		Qt::BlockingQueuedConnection, Q_ARG(const QByteArray&, userAgent)));
}

const UniqueLinkStore* Crawler::uniqueLinkStore() const noexcept
{
	return m_uniqueLinkStore;
}

ICrawlerOptions* Crawler::options() const noexcept
{
	return m_options;
}

Session::State Crawler::sessionState() const noexcept
{
	if (!m_session)
	{
		return Session::StateNone;
	}

	return m_session->state();
}

QString Crawler::sessionName() const
{
	if (!m_session)
	{
		return QString();
	}

	return m_session->name();
}

bool Crawler::hasCustomSessionName() const noexcept
{
	if (!m_session)
	{
		return false;
	}

	return m_session->hasCustomName();
}


bool Crawler::hasSession() const noexcept
{
	return m_session;
}

size_t Crawler::scannedPagesCount() const
{
	return m_uniqueLinkStore->crawledCount();
}

size_t Crawler::pagesCountOnSite() const
{
	return m_uniqueLinkStore->pendingCount();
}

void Crawler::addCustomDataFeed(ICustomDataFeed* customDataFeed)
{
	m_customDataFeeds[customDataFeed->name()] = customDataFeed;

	setCustomDataFeedsToSequencedDataCollection();
}

ICustomDataFeed* Crawler::customDataFeedByName(const QString& dataFeedName) const
{
	return m_customDataFeeds.value(dataFeedName, nullptr);
}

QList<ICustomDataFeed*> Crawler::customDataFeeds() const
{
	return m_customDataFeeds.values();
}

void Crawler::setWorkerCount(unsigned workerCount) noexcept
{
	m_theradCount = workerCount;
}

bool Crawler::crawlingFinished() const noexcept
{
	return m_crawlingFinished;
}

bool Crawler::readyForRefreshPage() const noexcept
{
	return state() == StatePause || state() == StatePending;
}

}
