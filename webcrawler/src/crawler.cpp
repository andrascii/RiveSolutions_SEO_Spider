#include "crawler.h"
#include "unordered_data_collection.h"
#include "sequenced_data_collection.h"
#include "model_controller.h"
#include "crawler_worker_thread.h"
#include "queued_downloader.h"

namespace WebCrawler
{

Crawler::Crawler(unsigned int threadCount)
	: QObject(nullptr)
	, m_theradCount(threadCount)
{
	m_crawlerThread = new QThread();
	moveToThread(m_crawlerThread);
	m_crawlerThread->start();

	m_modelController = new ModelController(this);
	ASSERT(qRegisterMetaType<ParsedPagePtr>());

	ASSERT(qRegisterMetaType<CrawlerOptions>() > -1);
}

Crawler::~Crawler()
{
	for (auto& worker : m_workers)
	{
		worker->stopExecution();
	}

	// Dtor should be called from a different thread
	ASSERT(thread() != QThread::currentThread());
	m_crawlerThread->quit();
	m_crawlerThread->wait();
	m_crawlerThread->deleteLater();
}

void Crawler::startCrawling(const CrawlerOptions& options)
{
	initCrawlerWorkerThreads();
	VERIFY(QMetaObject::invokeMethod(this, "startCrawlingInternal", Qt::QueuedConnection, Q_ARG(const CrawlerOptions&, options)));
}

void Crawler::stopCrawling()
{
	VERIFY(QMetaObject::invokeMethod(this, "stopCrawlingInternal", Qt::QueuedConnection));
}

void Crawler::startCrawlingInternal(const CrawlerOptions& options)
{
	m_modelController->setWebCrawlerOptions(options);

	DEBUG_ASSERT(options.host.isValid());

	INFOLOG << "crawler started";

	queuedDownloader()->start();
	m_urlStorage.setHost(options.host);

	for (std::unique_ptr<CrawlerWorkerThread>& worker : m_workers)
	{
		VERIFY(QMetaObject::invokeMethod(worker.get(), "applyOptions", Qt::QueuedConnection, Q_ARG(const CrawlerOptions&, options)));

		worker->startExecution();
	}
}

void Crawler::stopCrawlingInternal()
{
	INFOLOG << "crawler stopped";

	for (std::unique_ptr<CrawlerWorkerThread>& worker : m_workers)
	{
		worker->stopExecution();
	}

	queuedDownloader()->stop();
}

void Crawler::initCrawlerWorkerThreads()
{
	if (static_cast<unsigned int>(m_workers.size()) == m_theradCount)
	{
		return;
	}

	for (unsigned i = 0; i < m_theradCount; ++i)
	{
		m_workers.push_back(std::make_unique<CrawlerWorkerThread>(&m_urlStorage, queuedDownloader()));

		VERIFY(connect(m_workers[i].get(), SIGNAL(pageParsed(ParsedPagePtr)),
			SLOT(onPageParsed(ParsedPagePtr))));
	}
}

IQueuedDownloader* Crawler::queuedDownloader() const noexcept
{
	if (!m_queuedDownloader)
	{
		m_queuedDownloader.reset(createQueuedDownloader());
	}
	return m_queuedDownloader.get();
}

IQueuedDownloader* Crawler::createQueuedDownloader() const noexcept
{
	return new QueuedDownloader();
}

SequencedDataCollection* Crawler::guiStorage() const noexcept
{
	return m_modelController->data()->sequencedDataCollection();
}

const CrawlerUrlStorage* Crawler::crawlerUrlStorage() const noexcept
{
	return &m_urlStorage;
}

void Crawler::onPageParsed(ParsedPagePtr pageRaw)
{
	m_modelController->addParsedPage(pageRaw);
}

}
