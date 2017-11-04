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

namespace CrawlerEngine
{

Crawler::Crawler(unsigned int threadCount, QObject* parent)
	: QObject(parent)
	, m_robotsTxtLoader(nullptr)
	, m_modelController(nullptr)
	, m_uniqueLinkStore(new UniqueLinkStore(this))
	, m_theradCount(threadCount)
	, m_crawlingStateTimer(new QTimer(this))
	, m_sequencedDataCollection(nullptr)
{
	ASSERT(qRegisterMetaType<ParsedPagePtr>());
	ASSERT(qRegisterMetaType<CrawlingState>());
	ASSERT(qRegisterMetaType<CrawlerOptions>() > -1);
	ASSERT(qRegisterMetaType<RobotsTxtRules>());

	VERIFY(connect(m_crawlingStateTimer, &QTimer::timeout, this, &Crawler::onAboutCrawlingState));
	
	m_crawlingStateTimer->setInterval(100);
}

Crawler::~Crawler()
{
	//
	// IMPORTANT: First must be stopped workers!!!
	// Because workers sends requests to queuedDownloader
	// And it can not be stopped until it finishes processing requests
	//
	for (CrawlerWorkerThread* worker : m_workers)
	{
		VERIFY(QMetaObject::invokeMethod(worker, "stop", Qt::BlockingQueuedConnection));
	}

	ThreadManager::destroy();
}

void Crawler::initialize()
{
	m_modelController = new ModelController;

	ThreadManager::instance().moveObjectToThread(createDownloader()->qobject(), "DownloaderThread");
	ThreadManager::instance().moveObjectToThread(new HostInfoProvider, "BackgroundThread");
	ThreadManager::instance().moveObjectToThread(m_modelController, "BackgroundThread");

	for (unsigned i = 0; i < m_theradCount; ++i)
	{
		m_workers.push_back(new CrawlerWorkerThread(m_uniqueLinkStore));

		VERIFY(connect(m_workers.back(), SIGNAL(pageParsed(ParsedPagePtr)),
			m_modelController, SLOT(addParsedPage(ParsedPagePtr)), Qt::QueuedConnection));

		ThreadManager::instance().moveObjectToThread(m_workers.back(),
			QString("CrawlerWorkerThread#%1").arg(i).toLatin1());
	}
}

void Crawler::startCrawling(const CrawlerOptions& options)
{
	m_options = options;

	initializeCrawlingSession();
}

void Crawler::stopCrawling()
{
	for (CrawlerWorkerThread* worker : m_workers)
	{
		VERIFY(QMetaObject::invokeMethod(worker, "stop", Qt::BlockingQueuedConnection));
	}

	m_crawlingStateTimer->stop();

	emit crawlerStopped();

	INFOLOG << "crawler stopped";
}

void Crawler::onAboutCrawlingState()
{
	CrawlingState state;

	state.crawledLinkCount = uniqueLinkStore()->crawledLinksCount();
	state.pendingLinkCount = uniqueLinkStore()->pendingLinksCount();

	emit crawlingState(state);
}

void Crawler::onCrawlingSessionInitialized()
{
	if (!isPreinitialized())
	{
		return;
	}

	VERIFY(QMetaObject::invokeMethod(m_modelController, "setWebCrawlerOptions", 
		Qt::BlockingQueuedConnection, Q_ARG(const CrawlerOptions&, m_options)));

	m_uniqueLinkStore->addUrl(m_options.host, RequestTypeGet);

	for (CrawlerWorkerThread* worker : m_workers)
	{
		VERIFY(QMetaObject::invokeMethod(worker, "startWithOptions", Qt::QueuedConnection, 
			Q_ARG(const CrawlerOptions&, m_options), Q_ARG(RobotsTxtRules, RobotsTxtRules(robotsTxtLoader()->content()))));
	}

	m_crawlingStateTimer->start();

	emit crawlerStarted();

	INFOLOG << "crawler started";
}

bool Crawler::isPreinitialized() const
{
	return robotsTxtLoader()->isReady();
}

void Crawler::initializeCrawlingSession()
{
	DEBUG_ASSERT(m_options.host.isValid());

	VERIFY(connect(robotsTxtLoader()->qobject(), SIGNAL(ready()), 
		this, SLOT(onCrawlingSessionInitialized()), Qt::QueuedConnection));

	robotsTxtLoader()->load(m_options.host);
}

void Crawler::createSequencedDataCollection(QThread* targetThread)
{
	m_sequencedDataCollection.reset(m_modelController->data()->createSequencedDataCollection(targetThread));
}

IRobotsTxtLoader* Crawler::robotsTxtLoader() const
{
	if (!m_robotsTxtLoader)
	{
		m_robotsTxtLoader.reset(createRobotsTxtLoader());

		m_robotsTxtLoader->setUserAgent(m_options.plainUserAgent);
	}

	return m_robotsTxtLoader.get();
}

IDownloader* Crawler::createDownloader() const
{
	IDownloader* queuedDownloader = new Downloader;

	//queuedDownloader->setUserAgent(m_options.plainUserAgent);

	return queuedDownloader;
}

IRobotsTxtLoader* Crawler::createRobotsTxtLoader() const
{
	return new RobotsTxtLoader(new QNetworkAccessManager(const_cast<Crawler*>(this)));
}

SequencedDataCollection* Crawler::sequencedDataCollection()
{
	if(!m_sequencedDataCollection)
	{
		createSequencedDataCollection(QThread::currentThread());
	}

	return m_sequencedDataCollection.get();
}

const UniqueLinkStore* Crawler::uniqueLinkStore() const noexcept
{
	return m_uniqueLinkStore;
}

}
