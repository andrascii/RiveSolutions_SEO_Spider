#include "web_crawler.h"
#include "data_collection.h"
#include "model_controller.h"

namespace QuickieWebBot
{

WebCrawler::WebCrawler(unsigned int threadCount, ModelController* modelController, QObject* parent)
	: QObject(parent)
	, m_modelController(modelController)
{
	qRegisterMetaType<PageInfoPtr>();

	for (unsigned i = 0; i < threadCount; ++i)
	{
		m_workers.push_back(std::make_unique<PageInfoAcceptor>(&m_internalUrlStorage, &m_queuedDownloader));
		
		VERIFY(connect(m_workers[i].get(), SIGNAL(pageParsed(PageInfoPtr)), 
			SLOT(onPageInfoParsed(PageInfoPtr))));
	}
}

WebCrawler::~WebCrawler()
{
	for (auto& worker : m_workers)
	{
		worker->stop();
	}
}

void WebCrawler::startCrawling()
{
	INFOLOG("crawler", "crawler started");

	m_queuedDownloader.start();
	m_internalUrlStorage.setHost(m_modelController->host());

	for (std::unique_ptr<PageInfoAcceptor>& worker : m_workers)
	{
		worker->start();
	}
}

void WebCrawler::stopCrawling()
{
	INFOLOG("crawler", "crawler stopped");

	for (std::unique_ptr<PageInfoAcceptor>& worker : m_workers)
	{
		worker->stop();
	}

	m_queuedDownloader.stop();
}

void WebCrawler::onPageInfoParsed(PageInfoPtr pageInfo)
{
	m_modelController->addPageInfo(pageInfo);
}

}