#include "crawler_worker_thread.h"

namespace QuickieWebBot
{

CrawlerWorkerThread::CrawlerWorkerThread(QObject* parent)
	: QObject(parent)
{
}

void CrawlerWorkerThread::start(IModelController* pModelController)
{
	while (!m_onAboutToStop.load())
	{
		QUrl const& url = pModelController->url();

		m_networkRequest.setUrl(url);
	}
}

}