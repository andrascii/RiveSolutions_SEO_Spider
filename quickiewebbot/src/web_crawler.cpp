#include "web_crawler.h"
#include "page_info_acceptor.h"
#include "data_collection.h"
#include "model_controller.h"

namespace QuickieWebBot
{

WebCrawler::WebCrawler(unsigned int threadCount, ModelController* modelController, QObject* parent)
	: QObject(parent)
	, m_modelController(modelController)
	, m_workers(threadCount)
{
	qRegisterMetaType<PageInfoPtr>();

	for (unsigned i = 0; i < threadCount; ++i)
	{
		m_workers[i].first = new QThread(this);
		m_workers[i].second = new PageInfoAcceptor(&m_storage, &m_queuedDownloader);
		m_workers[i].second->moveToThread(m_workers[i].first);
		
		VERIFY(connect(m_workers[i].second, SIGNAL(pageParsed(PageInfoPtr)), 
			SLOT(onPageInfoParsed(PageInfoPtr))));

		m_workers[i].first->start();
	}
}

WebCrawler::~WebCrawler()
{
	for (const auto& pair : m_workers)
	{
		pair.second->stop();
		pair.first->quit();
		pair.first->wait();
	}
}

void WebCrawler::start()
{
	m_queuedDownloader.start();
	m_storage.setHost(m_modelController->host());

	for (const auto& pair : m_workers)
	{
		QMetaObject::invokeMethod(pair.second, "start");
	}
}

void WebCrawler::stop()
{
	for (const auto& pair : m_workers)
	{
		QMetaObject::invokeMethod(pair.second, "stop");
	}

	m_queuedDownloader.stop();
}

void WebCrawler::onPageInfoParsed(PageInfoPtr pageInfo)
{
	m_modelController->addPageInfo(pageInfo);
}

}