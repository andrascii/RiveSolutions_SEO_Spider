#include "crawler.h"
#include "unordered_data_collection.h"
#include "sequenced_data_collection.h"
#include "model_controller.h"
#include "crawler_worker_thread.h"
#include "queued_downloader.h"
#include "robots_txt_rules.h"
#include "robots_txt_loader.h"

namespace WebCrawler
{

Crawler::Crawler(unsigned int threadCount, QThread* sequencedDataCollectionThread)
	: QObject(nullptr)
	, m_modelController(new ModelController(this, sequencedDataCollectionThread))
	, m_theradCount(threadCount)
	, m_crawlingStateTimer(new QTimer(this))
	, m_robotsTxtLoader(nullptr)
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
	for (auto& worker : m_workers)
	{
		worker->stopExecution();
	}

	queuedDownloader()->stop();
}

void Crawler::startCrawling(const CrawlerOptions& options)
{
	m_options = options;

	VERIFY(QMetaObject::invokeMethod(this, "startCrawlingInternal", Qt::QueuedConnection));
}

void Crawler::stopCrawling()
{
	VERIFY(QMetaObject::invokeMethod(this, "stopCrawlingInternal", Qt::QueuedConnection));
}

void Crawler::startCrawlingInternal()
{
	initializeCrawlingSession();
}

void Crawler::stopCrawlingInternal()
{
	for (std::unique_ptr<CrawlerWorkerThread>& worker : m_workers)
	{
		worker->stopExecution();
	}

	queuedDownloader()->stop();

	m_crawlingStateTimer->stop();

	INFOLOG << "crawler stopped";
}

void Crawler::onAboutCrawlingState()
{
	CrawlingState state;

	state.crawledLinkCount = crawlerUrlStorage()->crawledLinksCount();
	state.pendingLinkCount = crawlerUrlStorage()->pendingLinksCount() + 
		queuedDownloader()->unprocessedRequestCount();

	emit crawlingState(state);
}

void Crawler::onCrawlingSessionInitialized()
{
	if (!isPreinitialized())
	{
		return;
	}

	m_modelController->setWebCrawlerOptions(m_options);
	m_uniqueLinkStore.setHost(m_options.host);

	queuedDownloader()->start();

	for (std::unique_ptr<CrawlerWorkerThread>& worker : m_workers)
	{
		VERIFY(QMetaObject::invokeMethod(worker.get(), "applyOptions", Qt::QueuedConnection, 
			Q_ARG(const CrawlerOptions&, m_options),
			Q_ARG(RobotsTxtRules, RobotsTxtRules(robotsTxtLoader()->content())))
		);

		worker->startExecution();
	}

	m_crawlingStateTimer->start();

	INFOLOG << "crawler started";
}

void Crawler::initCrawlerWorkerThreads()
{
	if (static_cast<unsigned int>(m_workers.size()) == m_theradCount)
	{
		return;
	}

	for (unsigned i = 0; i < m_theradCount; ++i)
	{
		m_workers.push_back(std::make_unique<CrawlerWorkerThread>(&m_uniqueLinkStore, queuedDownloader()));

		VERIFY(connect(m_workers[i].get(), SIGNAL(pageParsed(ParsedPagePtr)),
			SLOT(onPageParsed(ParsedPagePtr)), Qt::QueuedConnection));
	}
}

bool Crawler::isPreinitialized() const
{
	return robotsTxtLoader()->isReady();
}

void Crawler::initializeCrawlingSession()
{
	DEBUG_ASSERT(m_options.host.isValid());

	VERIFY(connect(robotsTxtLoader()->qobject(), SIGNAL(ready()), this, SLOT(onCrawlingSessionInitialized()), Qt::QueuedConnection));

	robotsTxtLoader()->load(m_options.host);

	initCrawlerWorkerThreads();
}

IQueuedDownloader* Crawler::queuedDownloader() const noexcept
{
	if (!m_queuedDownloader)
	{
		m_queuedDownloader.reset(createQueuedDownloader());
	}
	return m_queuedDownloader.get();
}

IRobotsTxtLoader* Crawler::robotsTxtLoader() const noexcept
{
	if (!m_robotsTxtLoader)
	{
		m_robotsTxtLoader.reset(createRobotsTxtLoader());

		m_robotsTxtLoader->setUserAgent(m_options.plainUserAgent);
	}

	return m_robotsTxtLoader.get();
}

IQueuedDownloader* Crawler::createQueuedDownloader() const noexcept
{
	IQueuedDownloader* queuedDownloader = new QueuedDownloader();
	queuedDownloader->setUserAgent(m_options.plainUserAgent);

	return queuedDownloader;
}

IRobotsTxtLoader* Crawler::createRobotsTxtLoader() const noexcept
{
	return new RobotsTxtLoader(new QNetworkAccessManager(const_cast<Crawler*>(this)));
}

SequencedDataCollection* Crawler::sequencedDataCollection() const noexcept
{
	return m_modelController->data()->sequencedDataCollection();
}

const UniqueLinkStore* Crawler::crawlerUrlStorage() const noexcept
{
	return &m_uniqueLinkStore;
}

void Crawler::onPageParsed(ParsedPagePtr pageRaw)
{
	m_modelController->addParsedPage(pageRaw);
}

}
