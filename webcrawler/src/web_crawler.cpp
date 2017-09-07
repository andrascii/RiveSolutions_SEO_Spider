#include "web_crawler.h"
#include "data_collection.h"
#include "model_controller.h"
#include "page_raw_processor.h"

namespace WebCrawler
{

WebCrawler::WebCrawler(unsigned int threadCount, ModelController* modelController, QObject* parent)
	: QObject(parent)
	, m_modelController(modelController)
{
	qRegisterMetaType<PageRawPtr>();

	for (unsigned i = 0; i < threadCount; ++i)
	{
		m_workers.push_back(std::make_unique<PageRawProcessor>(&m_internalUrlStorage, &m_queuedDownloader));
		
		VERIFY(connect(m_workers[i].get(), SIGNAL(webPageParsed(PageRawPtr)),
			SLOT(onPageRawParsed(PageRawPtr))));
	}
}

WebCrawler::~WebCrawler()
{
	for (auto& worker : m_workers)
	{
		worker->stopExecution();
	}
}

void WebCrawler::startCrawling(const WebCrawlerOptions& options)
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

void WebCrawler::stopCrawling()
{
	INFOLOG << "crawler stopped";

	for (std::unique_ptr<PageRawProcessor>& worker : m_workers)
	{
		worker->stopExecution();
	}

	m_queuedDownloader.stopExecution();
}

void WebCrawler::onPageRawParsed(PageRawPtr pageRaw)
{
	m_modelController->addPageRaw(pageRaw);
}

}