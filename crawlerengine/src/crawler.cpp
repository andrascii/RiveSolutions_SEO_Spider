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
	, m_modelController(nullptr)
	, m_uniqueLinkStore(new UniqueLinkStore(this))
	, m_theradCount(threadCount)
	, m_crawlingStateTimer(new QTimer(this))
	, m_sequencedDataCollection(nullptr)
	, m_state(StatePending)
{
	ASSERT(s_instance == nullptr && "Allowed only one instance of Crawler");

	ASSERT(qRegisterMetaType<ParsedPagePtr>());
	ASSERT(qRegisterMetaType<std::vector<ParsedPagePtr>>());
	ASSERT(qRegisterMetaType<CrawlingProgress>());
	ASSERT(qRegisterMetaType<CrawlerOptions>() > -1);
	ASSERT(qRegisterMetaType<RobotsTxtRules>());

	VERIFY(connect(m_crawlingStateTimer, &QTimer::timeout, this, &Crawler::onAboutCrawlingState));
	
	m_crawlingStateTimer->setInterval(100);

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
}

void Crawler::initialize()
{
	m_modelController = new ModelController;

	ThreadManager::instance().moveObjectToThread(createDownloader()->qobject(), "DownloaderThread");
	ThreadManager::instance().moveObjectToThread(new HostInfoProvider, "BackgroundThread");
	ThreadManager::instance().moveObjectToThread(m_modelController, "BackgroundThread");
	ThreadManager::instance().moveObjectToThread(createTaskProcessor()->qobject(), "BackgroundThread");


	for (unsigned i = 0; i < m_theradCount; ++i)
	{
		m_workers.push_back(new CrawlerWorkerThread(m_uniqueLinkStore));

		VERIFY(connect(m_workers.back(), SIGNAL(pageParsed(ParsedPagePtr)),
			m_modelController, SLOT(addParsedPage(ParsedPagePtr)), Qt::QueuedConnection));

		ThreadManager::instance().moveObjectToThread(m_workers.back(),
			QString("CrawlerWorkerThread#%1").arg(i).toLatin1());
	}

	VERIFY(connect(m_robotsTxtLoader->qobject(), SIGNAL(ready()),
		this, SLOT(onCrawlingSessionInitialized()), Qt::QueuedConnection));
}

void Crawler::clearData()
{
	ASSERT(m_state == StatePending || m_state == StatePause);

	VERIFY(QMetaObject::invokeMethod(m_modelController, "clearData", Qt::BlockingQueuedConnection));

	m_uniqueLinkStore->clear();

	m_state = StatePending;

	emit stateChanged(m_state);
	emit onAboutClearData();
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

	const size_t seqCollCount = m_sequencedDataCollection->crawledStorageSize();
	const size_t controllerCrawled = m_modelController->crawledStorageSize();
	const size_t controllerAccepted = m_modelController->acceptedCrawledStorageSize();

	const size_t linkStoreCrawled = uniqueLinkStore()->crawledLinksCount();
	const size_t linkStorePending = uniqueLinkStore()->pendingLinksCount();

	const size_t controllerPending = qMax(linkStoreCrawled, controllerCrawled) - controllerCrawled;
	const size_t seqCollPending = qMax(controllerAccepted, seqCollCount) - seqCollCount;
	const size_t additionalPendingCount = controllerPending + seqCollPending;

	progress.crawledLinkCount = linkStoreCrawled - additionalPendingCount;
	progress.pendingLinkCount = linkStorePending + additionalPendingCount;

	emit crawlingProgress(progress);
}

void Crawler::onCrawlingSessionInitialized()
{
	if (!isPreinitialized())
	{
		return;
	}

	VERIFY(QMetaObject::invokeMethod(m_modelController, "setWebCrawlerOptions", 
		Qt::BlockingQueuedConnection, Q_ARG(const CrawlerOptions&, m_options)));

	m_uniqueLinkStore->addUrl(m_options.host, DownloadRequestType::RequestTypeGet);

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
	return m_robotsTxtLoader->isReady();
}

void Crawler::initializeCrawlingSession()
{
	DEBUG_ASSERT(m_options.host.isValid());

	m_robotsTxtLoader->setUserAgent(m_options.plainUserAgent);
	m_robotsTxtLoader->load(m_options.host);
}

void Crawler::onSerializationTaskDone(Requester* requester, const TaskResponse& response)
{
	Q_UNUSED(requester);

	SerializationTaskResponseResult* result = dynamic_cast<SerializationTaskResponseResult*>(response.result.get());
	ASSERT(result != nullptr);
	m_state = m_prevState;
	emit stateChanged(m_state);
}

void Crawler::onDeserializationTaskDone(Requester* requester, const TaskResponse& response)
{
	Q_UNUSED(requester);

	SerializationTaskResponseResult* result = dynamic_cast<SerializationTaskResponseResult*>(response.result.get());
	ASSERT(result != nullptr);

	const std::vector<ParsedPagePtr>& pages = result->serializer->pages();

	for (const ParsedPagePtr& page : pages)
	{
		for (int i = 0; i < page->storages.size(); ++i)
		{
			if (page->storages[i])
			{
				m_modelController->data()->addParsedPage(page, static_cast<StorageType>(i));
			}
		}

	}

	m_uniqueLinkStore->setCrawledUrls(result->serializer->crawledLinks());
	m_uniqueLinkStore->setPendingUrls(result->serializer->pendingLinks());

	m_state = m_prevState;
	emit stateChanged(m_state);
}

void Crawler::createSequencedDataCollection(QThread* targetThread) const
{
	m_sequencedDataCollection.reset(m_modelController->data()->createSequencedDataCollection(targetThread));
}

IDownloader* Crawler::createDownloader() const
{
	IDownloader* downloader = new Downloader;

	downloader->setUserAgent(m_options.plainUserAgent);

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
		std::make_shared<Serializer>(std::move(pages), std::move(crawledUrls), std::move(pendingUrls)); 

	std::shared_ptr<ITask> task = std::make_shared<SerializationTask>(serializer, fileName);

	TaskRequest request(task);
	m_serializationRequester.reset(request, this, &Crawler::onSerializationTaskDone);
	m_serializationRequester->start();

	m_prevState = state();
	m_state = StateSerializaton;
	emit stateChanged(m_state);
}

void Crawler::loadFromFile(const QString& fileName)
{
	ASSERT(state() != State::StateWorking);
	clearData();

	std::shared_ptr<Serializer> serializer = std::make_shared<Serializer>();


	std::shared_ptr<ITask> task = std::make_shared<DeserializatoinTask>(serializer, fileName);

	TaskRequest request(task);
	m_deSerializationRequester.reset(request, this, &Crawler::onDeserializationTaskDone);
	m_deSerializationRequester->start();

	m_prevState = state();
	m_state = StateSerializaton;
	emit stateChanged(m_state);
}

const UniqueLinkStore* Crawler::uniqueLinkStore() const noexcept
{
	return m_uniqueLinkStore;
}

}
