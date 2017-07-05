#include "crawler_controller.h"
#include "crawler_page_info_acceptor.h"
#include "service_locator.h"

namespace QuickieWebBot
{

CrawlerController::CrawlerController(unsigned int threadCount, QObject* parent)
	: QObject(parent)
	, m_stop(false)
	, m_state(CrawlerPendingState)
	, m_workers(threadCount, nullptr)
{
	for (int i = 0; i < m_workers.size(); ++i)
	{
		m_workers[i] = new QThread(this);
		CrawlerPageInfoAcceptor* pageInfoAcceptor = new CrawlerPageInfoAcceptor;

		pageInfoAcceptor->moveToThread(m_workers[i]);

		VERIFY(connect(pageInfoAcceptor, SIGNAL(addElement(QThread*, WebSiteAnalyseElementPtr)),
			this, SLOT(onElementAdded(QThread*, WebSiteAnalyseElementPtr))));

		m_workers[i]->start();
	}
}

void CrawlerController::start()
{
	m_state = CrawlerWorkingState;
	m_stop = false;

	prepareAndStart();
}

void CrawlerController::stop()
{
	m_state = CrawlerPendingState;
	m_stop = true;
}

void CrawlerController::setThreadCount(unsigned int threadCount) noexcept
{
}

void CrawlerController::prepareAndStart() noexcept
{
	ServiceLocator* serviceLocator = ServiceLocator::instance();
	ModelController* modelController = serviceLocator->service<ModelController>();

	QUrl host = modelController->host();

	CrawlerPageInfoAcceptor* pageInfoAcceptor = new CrawlerPageInfoAcceptor(this);
	pageInfoAcceptor->handleUrl(host);


}

void CrawlerController::onElementAdded(QThread* fromThread, WebSiteAnalyseElementPtr element)
{
	//
	// write element to model controller and send new url to fromThread
	//
}

}