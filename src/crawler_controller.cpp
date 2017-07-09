#include "crawler_controller.h"
#include "crawler_worker_thread.h"

namespace QuickieWebBot
{

CrawlerController::CrawlerController()
{
}

void CrawlerController::startCrawlers(size_t num)
{
	for (size_t i = 0; i < num; ++i)
	{
		m_workerThreads.push_back(new QThread(this));

		CrawlerWorkerThread* workerThread = new CrawlerWorkerThread;
		workerThread->moveToThread(m_workerThreads[i]);

		connect(m_workerThreads[i], &QThread::finished, workerThread, &QObject::deleteLater);
	}
}

void CrawlerController::stopCrawlers()
{

}

}