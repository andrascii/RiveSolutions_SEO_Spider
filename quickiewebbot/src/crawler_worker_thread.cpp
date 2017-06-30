#include "crawler_worker_thread.h"

namespace QuickieWebBot
{

CrawlerWorkerThread::CrawlerWorkerThread(QObject* parent)
	: QObject(parent)
{
}

void CrawlerWorkerThread::start(ModelController* pModelController)
{
	while (!m_onAboutToStop.load())
	{
		
	}
}

}