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
	, m_serializatonRedyStateCheckerTimer(new QTimer(this))
	, m_sequencedDataCollection(nullptr)
	, m_state(StatePending)
	, m_downloader(nullptr)
{
	ServiceLocator* serviceLocator = ServiceLocator::instance();
	serviceLocator->addService<INotificationService>(new NotificationService);

	ASSERT(s_instance == nullptr && "Allowed only one instance of Crawler");
	ASSERT(qRegisterMetaType<ParsedPagePtr>());
	ASSERT(qRegisterMetaType<std::vector<ParsedPagePtr>>());
	ASSERT(qRegisterMetaType<CrawlingProgress>());
	ASSERT(qRegisterMetaType<CrawlerOptions>() > -1);
	ASSERT(qRegisterMetaType<RobotsTxtRules>());

	VERIFY(connect(m_crawlingStateTimer, &QTimer::timeout, this, &Crawler::onAboutCrawlingState));
	VERIFY(connect(m_serializatonRedyStateCheckerTimer, &QTimer::timeout, this, &Crawler::checkSerialiationReadyState));
	
	m_crawlingStateTimer->setInterval(100);
	m_serializatonRedyStateCheckerTimer->setInterval(200);

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
	m_downloader = createDownloader();

	ThreadManager::instance().moveObjectToThread(m_downloader->qobject(), "DownloaderThread");
	ThreadManager::instance().moveObjectToThread(new HostInfoProvider, "BackgroundThread");
	ThreadManager::instance().moveObjectToThread(m_modelController, "BackgroundThread");
	ThreadManager::instance().moveObjectToThread(createTaskProcessor()->qobject(), "BackgroundThread");

	for (unsigned i = 0; i < m_theradCount; ++i)
	{
		m_workers.push_back(new CrawlerWorkerThread(m_uniqueLinkStore));
		VERIFY(connect(m_workers.back(), SIGNAL(pageParsed(ParsedPagePtr)), m_modelController, SLOT(addParsedPage(ParsedPagePtr)), Qt::QueuedConnection));
		ThreadManager::instance().moveObjectToThread(m_workers.back(), QString("CrawlerWorkerThread#%1").arg(i).toLatin1());
	}

	VERIFY(connect(m_robotsTxtLoader->qobject(), SIGNAL(ready()), this, SLOT(onCrawlingSessionInitialized()), Qt::QueuedConnection));
	VERIFY(connect(m_xmlSitemapLoader->qobject(), SIGNAL(ready()), this, SLOT(onCrawlingSessionInitialized()), Qt::QueuedConnection));
}

void Crawler::clearData()
{
	ASSERT(m_state == StatePending || m_state == StatePause);

	clearDataImpl();

	m_state = StatePending;

	emit stateChanged(m_state);
	emit onAboutClearData();
}

void Crawler::clearDataImpl()
{
	VERIFY(QMetaObject::invokeMethod(m_modelController, "clearData", Qt::BlockingQueuedConnection));
	m_uniqueLinkStore->clear();
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
	m_state = StateWorking;
	initializeCrawlingSession();

	emit stateChanged(m_state);
}

void Crawler::stopCrawling()
{
	if (m_state == StatePause || m_state == StatePending)
	{
		return;
	}

	m_state = StatePause;

	for (CrawlerWorkerThread* worker : m_workers)
	{
		VERIFY(QMetaObject::invokeMethod(worker, "stop", Qt::BlockingQueuedConnection));
	}

	m_crawlingStateTimer->stop();

	emit crawlerStopped();
	emit stateChanged(m_state);

	INFOLOG << "crawler stopped";
}

void Crawler::onAboutCrawlingState()
{
	CrawlingProgress progress;

	const CrawlerSharedState* state = CrawlerSharedState::instance();

	const int seqCollCount = state->sequencedDataCollectionLinksCount();
	const int controllerCrawled = state->modelControllerCrawledLinksCount();
	const int controllerAccepted = state->modelControllerAcceptedLinksCount();
	const int linkStoreCrawled = state->downloaderCrawledLinksCount();
	const int linkStorePending = state->downloaderPendingLinksCount();

	const size_t controllerPending = qMax(linkStoreCrawled, controllerCrawled) - controllerCrawled;
	const size_t seqCollPending = qMax(controllerAccepted, seqCollCount) - seqCollCount;
	const size_t additionalPendingCount = controllerPending + seqCollPending;

	progress.crawledLinkCount = linkStoreCrawled - additionalPendingCount;
	progress.pendingLinkCount = linkStorePending + additionalPendingCount;

	emit crawlingProgress(progress);
}

void Crawler::checkSerialiationReadyState()
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

		m_serializatonRedyStateCheckerTimer->stop();
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

	if (!m_options.pauseRangeFrom && !m_options.pauseRangeTo)
	{
		VERIFY(QMetaObject::invokeMethod(m_downloader->qobject(), "resetPauseRange", Qt::BlockingQueuedConnection));
	}
	else
	{
		VERIFY(QMetaObject::invokeMethod(m_downloader->qobject(), "setPauseRange",
			Qt::BlockingQueuedConnection, Q_ARG(int, m_options.pauseRangeFrom), Q_ARG(int, m_options.pauseRangeTo)));
	}

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
		VERIFY(QMetaObject::invokeMethod(m_downloader->qobject(), "resetProxy",	Qt::BlockingQueuedConnection));
	}

	m_uniqueLinkStore->addUrl(m_options.startCrawlingPage, DownloadRequestType::RequestTypeGet);

	for (CrawlerWorkerThread* worker : m_workers)
	{
		VERIFY(QMetaObject::invokeMethod(worker, "startWithOptions", Qt::QueuedConnection, 
			Q_ARG(const CrawlerOptions&, m_options), Q_ARG(RobotsTxtRules, RobotsTxtRules(m_robotsTxtLoader->content()))));
	}

	m_crawlingStateTimer->start();

	emit crawlerStarted();

	INFOLOG << "crawler started";
}

bool Crawler::isPreinitialized() const
{
	return m_robotsTxtLoader->isReady() && m_xmlSitemapLoader->isReady();
}

void Crawler::initializeCrawlingSession()
{
	DEBUG_ASSERT(m_options.startCrawlingPage.isValid());

	m_robotsTxtLoader->setHost(m_options.startCrawlingPage);
	m_xmlSitemapLoader->setHost(m_options.startCrawlingPage);
	m_robotsTxtLoader->load();
	m_xmlSitemapLoader->load();
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
		serviceLocator->service<INotificationService>()->error(tr("Error"), tr("The operation has not been successful"));
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
		serviceLocator->service<INotificationService>()->error(tr("Error"), tr("The operation has not been successful"));
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

void Crawler::onSerializationReadyToBeStarted()
{
	ASSERT(state() == StateSerializaton);
	const SequencedDataCollection* sequencedCollection = sequencedDataCollection();
	const ISequencedStorage* storage = sequencedCollection->storage(StorageType::CrawledUrlStorageType);
	std::vector<ParsedPagePtr> pendingPages = m_modelController->data()->allParsedPages(StorageType::PendingResourcesStorageType);

	std::vector<ParsedPage*> pages;
	const int pagesCount = storage->size() + static_cast<int>(pendingPages.size());
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
		const std::vector<CrawlerRequest> workerPendingUrls = worker->pendingUrls();
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

void Crawler::createSequencedDataCollection(QThread* targetThread) const
{
	m_sequencedDataCollection.reset(m_modelController->data()->createSequencedDataCollection(targetThread));
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
	if(!m_sequencedDataCollection)
	{
		createSequencedDataCollection(QThread::currentThread());
	}

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
	m_prevState = state();
	m_state = StateSerializaton;
	emit stateChanged(m_state);

	m_serializatonRedyStateCheckerTimer->start();
	checkSerialiationReadyState();
}

void Crawler::loadFromFile(const QString& fileName)
{
	ASSERT(state() != State::StateWorking);
	
	m_fileName = fileName;
	m_prevState = state();
	m_state = StateDeserializaton;
	emit stateChanged(m_state);

	m_serializatonRedyStateCheckerTimer->start();
	checkSerialiationReadyState();
}

const ISpecificLoader* Crawler::robotsTxtLoader() const noexcept
{
	return m_robotsTxtLoader;
}

const CrawlerEngine::ISpecificLoader* Crawler::xmlSitemapLoader() const noexcept
{
	return m_xmlSitemapLoader;
}

const UniqueLinkStore* Crawler::uniqueLinkStore() const noexcept
{
	return m_uniqueLinkStore;
}

}
