#include "crawler.h"
#include "unordered_data_collection.h"
#include "sequenced_data_collection.h"
#include "model_controller.h"
#include "crawler_worker_thread.h"
#include "robots_txt_rules.h"
#include "robots_txt_loader.h"
#include "thread_manager.h"
#include "downloader.h"
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
#include "web_screenshot.h"
#include "host_info.h"
#include "isequenced_storage.h"


namespace CrawlerEngine
{

Crawler* Crawler::s_instance = nullptr;

Crawler& Crawler::instance()
{
	return *s_instance;
}

Crawler::Crawler(unsigned int threadCount, QObject* parent)
	: QObject(parent)
	, m_robotsTxtLoader(new RobotsTxtLoader(this))
	, m_xmlSitemapLoader(new XmlSitemapLoader(static_cast<RobotsTxtLoader*>(m_robotsTxtLoader), this))
	, m_modelController(nullptr)
	, m_uniqueLinkStore(new UniqueLinkStore(this))
	, m_theradCount(threadCount)
	, m_crawlingStateTimer(new QTimer(this))
	, m_serializatonReadyStateCheckerTimer(new QTimer(this))
	, m_sequencedDataCollection(nullptr)
	, m_state(StatePending)
	, m_downloader(nullptr)
	, m_webScreenShot(nullptr)
{
	ServiceLocator* serviceLocator = ServiceLocator::instance();
	serviceLocator->addService<INotificationService>(new NotificationService);

	ASSERT(s_instance == nullptr && "Allowed only one instance of Crawler");
	ASSERT(qRegisterMetaType<WorkerResult>());
	ASSERT(qRegisterMetaType<ParsedPagePtr>());
	ASSERT(qRegisterMetaType<std::vector<ParsedPagePtr>>());
	ASSERT(qRegisterMetaType<CrawlingProgress>());
	ASSERT(qRegisterMetaType<CrawlerOptions>() > -1);
	ASSERT(qRegisterMetaType<RobotsTxtRules>());

	VERIFY(connect(m_crawlingStateTimer, &QTimer::timeout, this, &Crawler::onAboutCrawlingState));
	VERIFY(connect(m_serializatonReadyStateCheckerTimer, &QTimer::timeout, this, &Crawler::waitSerializationReadyState));
	
	m_crawlingStateTimer->setInterval(100);
	m_serializatonReadyStateCheckerTimer->setInterval(200);

	s_instance = this;
}

Crawler::~Crawler()
{
	for (CrawlerWorkerThread* worker : m_workers)
	{
		VERIFY(QMetaObject::invokeMethod(worker, "stop", Qt::BlockingQueuedConnection));
	}

	ThreadManager::destroy();

	s_instance = nullptr;

	ServiceLocator::instance()->destroyService<INotificationService>();
}

void Crawler::initialize()
{
	m_modelController = new ModelController;

	initSequencedDataCollection();

	m_downloader = createDownloader();
	m_webScreenShot = createWebScreenShot();

	ThreadManager& threadManager = ThreadManager::instance();

	threadManager.moveObjectToThread(m_downloader->qobject(), "DownloaderThread");
	threadManager.moveObjectToThread(m_modelController, "BackgroundThread");
	threadManager.moveObjectToThread(createHostInfoProvider()->qobject(), "BackgroundThread");
	threadManager.moveObjectToThread(createTaskProcessor()->qobject(), "BackgroundThread");

	for (unsigned i = 0; i < m_theradCount; ++i)
	{
		m_workers.push_back(new CrawlerWorkerThread(m_uniqueLinkStore));

		VERIFY(connect(m_workers.back(), SIGNAL(workerResult(WorkerResult)), m_modelController, SLOT(handleWorkerResult(WorkerResult)), Qt::QueuedConnection));

		threadManager.moveObjectToThread(m_workers.back(), QString("CrawlerWorkerThread#%1").arg(i).toLatin1());
	}

	VERIFY(connect(m_robotsTxtLoader->qobject(), SIGNAL(ready()), this, SLOT(onCrawlingSessionInitialized()), Qt::QueuedConnection));
	VERIFY(connect(m_xmlSitemapLoader->qobject(), SIGNAL(ready()), this, SLOT(onCrawlingSessionInitialized()), Qt::QueuedConnection));
}

void Crawler::clearData()
{
	ASSERT(m_state == StatePending || m_state == StatePause);

	clearDataImpl();

	setState(StatePending);

	emit onAboutClearData();
}

void Crawler::clearDataImpl()
{
	VERIFY(QMetaObject::invokeMethod(m_modelController, "clearData", Qt::BlockingQueuedConnection));
	m_uniqueLinkStore->clear();
}

void Crawler::setState(State state)
{
	m_prevState = m_state;
	m_state = state;
	emit stateChanged(state);
}

bool Crawler::isNoData() const noexcept
{
	return m_sequencedDataCollection->empty();
}

CrawlerEngine::Crawler::State Crawler::state() const noexcept
{
	return m_state;
}

void Crawler::startCrawling(const CrawlerOptions& options)
{
	m_options = options;

	setState(StateWorking);

	if (!m_options.pauseRangeFrom && !m_options.pauseRangeTo)
	{
		VERIFY(QMetaObject::invokeMethod(m_downloader->qobject(), "resetPauseRange", Qt::BlockingQueuedConnection));
	}
	else
	{
		VERIFY(QMetaObject::invokeMethod(m_downloader->qobject(), "setPauseRange",
			Qt::BlockingQueuedConnection, Q_ARG(int, m_options.pauseRangeFrom), Q_ARG(int, m_options.pauseRangeTo)));
	}

	initializeCrawlingSession();
}

void Crawler::stopCrawling()
{
	if (m_state == StatePause || m_state == StatePending)
	{
		return;
	}

	setState(StatePause);

	for (CrawlerWorkerThread* worker : m_workers)
	{
		VERIFY(QMetaObject::invokeMethod(worker, "stop", Qt::BlockingQueuedConnection));
	}

	m_crawlingStateTimer->stop();

	emit crawlerStopped();

	ServiceLocator* serviceLocator = ServiceLocator::instance();
	serviceLocator->service<INotificationService>()->info(tr("Crawler"), tr("Crawler stopped"));
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

	progress.crawledLinkCount = uniqueLinkStoreCrawledCount - additionalPendingCount;
	progress.pendingLinkCount = uniqueLinkStorePendingCount + additionalPendingCount;

	emit crawlingProgress(progress);

	if (uniqueLinkStorePendingCount == 0 &&
		sequencedDataCollectionCount > 0 &&
		uniqueLinkStoreCrawledCount == state->workersProcessedLinksCount() &&
		state->modelControllerCrawledLinksCount() == state->workersProcessedLinksCount() &&
		modelControllerAcceptedLinksCount == sequencedDataCollectionCount)
	{
		stopCrawling();
		setState(StatePending);

		ServiceLocator* serviceLocator = ServiceLocator::instance();
		serviceLocator->service<INotificationService>()->info(tr("Crawler"), tr("Program has ended crawling."));
	}
}

void Crawler::waitSerializationReadyState()
{
	const CrawlerSharedState* state = CrawlerSharedState::instance();

	if ((m_state == StateSerializaton || m_state == StateDeserializaton) &&
		!m_fileName.isEmpty() &&
		state->workersProcessedLinksCount() == state->modelControllerCrawledLinksCount() &&
		state->modelControllerAcceptedLinksCount() == state->sequencedDataCollectionLinksCount())
	{
		if (m_state == StateSerializaton)
		{
			onSerializationReadyToBeStarted();
		}
		else if (m_state == StateDeserializaton)
		{
			onDeserializationReadyToBeStarted();
		}

		m_serializatonReadyStateCheckerTimer->stop();
	}
}

void Crawler::onCrawlingSessionInitialized()
{
	if (!isPreinitialized())
	{
		return;
	}

	VERIFY(QMetaObject::invokeMethod(m_modelController, "setWebCrawlerOptions", 
		Qt::BlockingQueuedConnection, Q_ARG(const CrawlerOptions&, m_options)));

	VERIFY(QMetaObject::invokeMethod(m_downloader->qobject(), "setUserAgent",
		Qt::BlockingQueuedConnection, Q_ARG(const QByteArray&, m_options.userAgent)));

	if (m_options.useProxy)
	{
		VERIFY(QMetaObject::invokeMethod(m_downloader->qobject(), "setProxy",
			Qt::BlockingQueuedConnection,
			Q_ARG(const QString&, m_options.proxyHostName),
			Q_ARG(int, m_options.proxyPort),
			Q_ARG(const QString&, m_options.proxyUser),
			Q_ARG(const QString&, m_options.proxyPassword)));
	}
	else
	{
		VERIFY(QMetaObject::invokeMethod(m_downloader->qobject(), "resetProxy", Qt::BlockingQueuedConnection));
	}

	m_uniqueLinkStore->addUrl(m_options.startCrawlingPage, DownloadRequestType::RequestTypeGet);

	for (CrawlerWorkerThread* worker : m_workers)
	{
		VERIFY(QMetaObject::invokeMethod(worker, "startWithOptions", Qt::QueuedConnection, 
			Q_ARG(const CrawlerOptions&, m_options), Q_ARG(RobotsTxtRules, RobotsTxtRules(m_robotsTxtLoader->content()))));
	}

	m_crawlingStateTimer->start();

	emit crawlerStarted();

	ServiceLocator* serviceLocator = ServiceLocator::instance();
	serviceLocator->service<INotificationService>()->info(tr("Crawler"), tr("Crawler started"));
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

	GetHostInfoRequest request(m_options.startCrawlingPage);
	m_hostInfoRequester.reset(request, this, &Crawler::onHostInfoResponse);
	m_hostInfoRequester->start();
}

void Crawler::onSerializationTaskDone(Requester* requester, const TaskResponse& response)
{
	Q_UNUSED(requester);

	SerializationTaskResponseResult* result = dynamic_cast<SerializationTaskResponseResult*>(response.result.get());

	ASSERT(result);

	m_state = m_prevState;
	emit stateChanged(m_state);

	if (!result->error.isEmpty())
	{
		ServiceLocator* serviceLocator = ServiceLocator::instance();
		ASSERT(serviceLocator->isRegistered<INotificationService>());
		serviceLocator->service<INotificationService>()->error(tr("Serialization error"), tr("The operation has not been successful."));
	}

	m_serializationRequester.reset();
	emit serializationProcessDone();
}

void Crawler::onDeserializationTaskDone(Requester* requester, const TaskResponse& response)
{
	Q_UNUSED(requester);

	SerializationTaskResponseResult* result = dynamic_cast<SerializationTaskResponseResult*>(response.result.get());

	ASSERT(result);

	if (!result->error.isEmpty())
	{
		ServiceLocator* serviceLocator = ServiceLocator::instance();
		ASSERT(serviceLocator->isRegistered<INotificationService>());
		serviceLocator->service<INotificationService>()->error(tr("Deserialization error"), tr("The operation has not been successful."));
	}
	else
	{
		const std::vector<ParsedPagePtr>& pages = result->serializer->pages();

		int crawledLinksCount = 0;

		for (const ParsedPagePtr& page : pages)
		{
			for (int i = 0; i < page->storages.size(); ++i)
			{
				if (page->storages[i])
				{
					const StorageType storage = static_cast<StorageType>(i);

					if (storage == StorageType::CrawledUrlStorageType)
					{
						++crawledLinksCount;
					}

					m_modelController->data()->addParsedPage(page, static_cast<StorageType>(i));
				}
			}
		}

		m_uniqueLinkStore->setCrawledUrls(result->serializer->crawledLinks());
		m_uniqueLinkStore->setPendingUrls(result->serializer->pendingLinks());

		m_options = result->serializer->crawlerOptions();
		emit crawlerOptionsChanged(m_options);

		CrawlerSharedState* state = CrawlerSharedState::instance();
		state->setDownloaderCrawledLinksCount(static_cast<int>(result->serializer->crawledLinks().size()));
		state->setDownloaderPendingLinksCount(static_cast<int>(result->serializer->pendingLinks().size()));
		state->setWorkersProcessedLinksCount(crawledLinksCount);
		state->setModelControllerAcceptedLinksCount(crawledLinksCount);
		state->setModelControllerCrawledLinksCount(crawledLinksCount);
	}

	m_state = m_prevState;
	emit stateChanged(m_state);

	m_deSerializationRequester.reset();

	emit deserializationProcessDone();
}

void Crawler::onHostInfoResponse(Requester*, const GetHostInfoResponse& response)
{
	m_hostInfoRequester->stop();

	if (!response.hostInfo.isValid())
	{
		ServiceLocator* serviceLocator = ServiceLocator::instance();

		serviceLocator->service<INotificationService>()->error(
			tr("DNS Lookup Failed!"), 
			tr("I'm sorry but I cannot find this website.\n"
				"Please, be sure that you entered a valid address.")
		);

		setState(StatePending);

		return;
	}

	m_hostInfo.reset(new HostInfo(response.hostInfo));
	m_options.startCrawlingPage = response.url;

	m_webScreenShot->load(m_options.startCrawlingPage);

	tryToLoadCrawlingDependencies();
}

void Crawler::onSerializationReadyToBeStarted()
{
	ASSERT(state() == StateSerializaton);

	const SequencedDataCollection* sequencedCollection = sequencedDataCollection();

	const ISequencedStorage* storage = sequencedCollection->storage(StorageType::CrawledUrlStorageType);
	std::vector<ParsedPagePtr> pendingPages = m_modelController->data()->allParsedPages(StorageType::PendingResourcesStorageType);

	const int pagesCount = storage->size() + static_cast<int>(pendingPages.size());

	std::vector<ParsedPage*> pages;
	pages.reserve(pagesCount);

	for (int i = 0; i < storage->size(); ++i)
	{
		const ParsedPage* page = (*storage)[i];
		pages.push_back(const_cast<ParsedPage*>(page));
	}

	for (int i = 0; i < pendingPages.size(); ++i)
	{
		ParsedPage* page = pendingPages[i].get();
		pages.push_back(page);
	}

	std::vector<CrawlerRequest> pendingUrls;

	for (CrawlerWorkerThread* worker : m_workers)
	{
		std::future<std::vector<CrawlerRequest>> pendingUrlsFuture = worker->pendingUrls();
		const std::vector<CrawlerRequest> workerPendingUrls = pendingUrlsFuture.get();

		pendingUrls.insert(pendingUrls.end(), workerPendingUrls.begin(), workerPendingUrls.end());
	}

	std::vector<CrawlerRequest> linkStorePendingUrls = m_uniqueLinkStore->pendingUrls();
	pendingUrls.insert(pendingUrls.end(), linkStorePendingUrls.begin(), linkStorePendingUrls.end());

	std::vector<CrawlerRequest> crawledUrls = m_uniqueLinkStore->crawledUrls();

	std::shared_ptr<Serializer> serializer =
		std::make_shared<Serializer>(std::move(pages), std::move(crawledUrls), std::move(pendingUrls), m_options);

	std::shared_ptr<ITask> task = std::make_shared<SerializationTask>(serializer, m_fileName);
	m_fileName = QString();

	TaskRequest request(task);
	m_serializationRequester.reset(request, this, &Crawler::onSerializationTaskDone);
	m_serializationRequester->start();
}

void Crawler::onDeserializationReadyToBeStarted()
{
	ASSERT(state() == StateDeserializaton);

	clearDataImpl();

	std::shared_ptr<Serializer> serializer = std::make_shared<Serializer>();
	std::shared_ptr<ITask> task = std::make_shared<DeserializatoinTask>(serializer, m_fileName);
	m_fileName = QString();

	TaskRequest request(task);
	m_deSerializationRequester.reset(request, this, &Crawler::onDeserializationTaskDone);
	m_deSerializationRequester->start();
}

void Crawler::tryToLoadCrawlingDependencies() const
{
	DEBUG_ASSERT(m_options.startCrawlingPage.isValid());

	m_robotsTxtLoader->setHost(m_options.startCrawlingPage);
	m_xmlSitemapLoader->setHost(m_options.startCrawlingPage);
	m_robotsTxtLoader->load();
	m_xmlSitemapLoader->load();
}

void Crawler::initSequencedDataCollection()
{
	m_sequencedDataCollection = std::make_unique<SequencedDataCollection>(m_modelController->data());
	m_sequencedDataCollection->initialize();
}

IHostInfoProvider* Crawler::createHostInfoProvider() const
{
	return new HostInfoProvider;
}

CrawlerEngine::IWebScreenShot* Crawler::createWebScreenShot()
{
	return new WebScreenShot(this);
}

IDownloader* Crawler::createDownloader() const
{
	IDownloader* downloader = new Downloader;

	return downloader;
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

void Crawler::saveToFile(const QString& fileName)
{
	ASSERT(state() != State::StateWorking);

	m_fileName = fileName;
	setState(StateSerializaton);

	m_serializatonReadyStateCheckerTimer->start();
	waitSerializationReadyState();
}

void Crawler::loadFromFile(const QString& fileName)
{
	if (state() == Crawler::StateWorking)
	{
		ServiceLocator* serviceLocator = ServiceLocator::instance();
		serviceLocator->service<INotificationService>()->info(tr("Error"), tr("Cannot open a document while crawler is working!"));

		return;
	}
	
	m_fileName = fileName;
	setState(StateDeserializaton);

	m_serializatonReadyStateCheckerTimer->start();
	waitSerializationReadyState();
}

const ISpecificLoader* Crawler::robotsTxtLoader() const noexcept
{
	return m_robotsTxtLoader;
}

const CrawlerEngine::ISpecificLoader* Crawler::xmlSitemapLoader() const noexcept
{
	return m_xmlSitemapLoader;
}

const QPixmap& Crawler::currentCrawledSitePixmap() const noexcept
{
	return m_webScreenShot->result();
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

void Crawler::refreshPage(StorageType storageType, int index)
{
	ASSERT(state() == StatePause || state() == StatePending);
	ASSERT(!m_workers.empty());

	ParsedPage* parsedPage = m_sequencedDataCollection->storage(storageType)->get(index);

	for (StorageType type = StorageType::CrawledUrlStorageType; type < StorageType::EndEnumStorageType; type = ++type)
	{
		if (type == StorageType::CrawledUrlStorageType)
		{
			continue;
		}

		m_sequencedDataCollection->removePage(parsedPage, type);
	}

	VERIFY(QMetaObject::invokeMethod(m_modelController, "preparePageForRefresh", 
		Qt::BlockingQueuedConnection, Q_ARG(ParsedPage*, parsedPage)));

	m_uniqueLinkStore->addRefreshUrl(parsedPage->url, DownloadRequestType::RequestTypeGet);
}

const UniqueLinkStore* Crawler::uniqueLinkStore() const noexcept
{
	return m_uniqueLinkStore;
}

bool Crawler::canRefreshPage() const noexcept
{
	return state() == StatePause || state() == StatePending;
}

}
