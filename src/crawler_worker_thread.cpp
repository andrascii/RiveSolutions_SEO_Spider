#include "crawler_worker_thread.h"
#include "service_locator.h"

namespace QuickieWebBot
{

CrawlerWorkerThread::CrawlerWorkerThread(QObject* parent)
	: QObject(parent)
	, m_onAboutToStop(false)
	, m_modelController(nullptr)
{
}

void CrawlerWorkerThread::setupModelController(ModelController* pModelController)
{
	m_modelController = pModelController;
}

void CrawlerWorkerThread::start()
{
	assert(m_modelController);

	QNetworkAccessManager* pNetworkAccessManager = 
		ServiceLocator::instance()->service<QNetworkAccessManager>();



	while (!m_onAboutToStop.load())
	{
		pNetworkAccessManager->get(QNetworkRequest());



		QMetaObject::invokeMethod(m_modelController, "addWebsiteAnalyseElement",
			Q_ARG(std::shared_ptr<WebsiteAnalyseElement>, std::shared_ptr<WebsiteAnalyseElement>(nullptr)));
	}
}

}