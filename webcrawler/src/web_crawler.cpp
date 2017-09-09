#include "web_crawler.h"
#include "data_collection.h"
#include "gui_storage.h"
#include "model_controller.h"
#include "page_raw_processor.h"

namespace WebCrawler
{

WebCrawler::WebCrawler(unsigned int threadCount)
	: QObject(nullptr)
{
	m_crawlerThread = new QThread();
	this->moveToThread(m_crawlerThread);
	m_crawlerThread->start();

	m_modelController = new ModelController(this);
	qRegisterMetaType<PageRawPtr>();

	for (unsigned i = 0; i < threadCount; ++i)
	{
		m_workers.push_back(std::make_unique<PageRawProcessor>(&m_internalUrlStorage, &m_queuedDownloader));
		
		VERIFY(connect(m_workers[i].get(), SIGNAL(webPageParsed(PageRawPtr)),
			SLOT(onPageRawParsed(PageRawPtr))));
	}

	ASSERT(qRegisterMetaType<WebCrawlerOptions>() > -1);
}

WebCrawler::~WebCrawler()
{
	for (auto& worker : m_workers)
	{
		worker->stopExecution();
	}

	// Dtor should be called from a different thread
	ASSERT(this->thread() != QThread::currentThread());
	m_crawlerThread->quit();
	m_crawlerThread->wait();
	m_crawlerThread->deleteLater();
}

void WebCrawler::startCrawling(const WebCrawlerOptions& options)
{
	VERIFY(QMetaObject::invokeMethod(this, "startCrawlingInternal", Qt::QueuedConnection, Q_ARG(const WebCrawlerOptions&, options)));
}

void WebCrawler::stopCrawling()
{
	VERIFY(QMetaObject::invokeMethod(this, "stopCrawlingInternal", Qt::QueuedConnection));
}

void WebCrawler::startCrawlingInternal(const WebCrawlerOptions& options)
{
	m_modelController->setWebCrawlerOptions(options);

	QUrl host = options.url;

	DEBUG_ASSERT(host.isValid());

	INFOLOG << "crawler started";

	m_queuedDownloader.startExecution();
	m_internalUrlStorage.setHost(host);

	for (std::unique_ptr<PageRawProcessor>& worker : m_workers)
	{
		VERIFY(QMetaObject::invokeMethod(worker.get(), "setHost", Qt::QueuedConnection, Q_ARG(QUrl, host)));

		worker->startExecution();
	}
}

void WebCrawler::stopCrawlingInternal()
{
	INFOLOG << "crawler stopped";

	for (std::unique_ptr<PageRawProcessor>& worker : m_workers)
	{
		worker->stopExecution();
	}

	m_queuedDownloader.stopExecution();
}

GuiStorage* WebCrawler::guiStorage() const noexcept
{
	return m_modelController->data()->guiStorage();
}

void WebCrawler::onPageRawParsed(PageRawPtr pageRaw)
{
	m_modelController->addPageRaw(pageRaw);
}

}