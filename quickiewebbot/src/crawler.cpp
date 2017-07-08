#include "crawler.h"
#include "crawler_page_info_acceptor.h"
#include "service_locator.h"
#include "model_controller_data.h"

namespace QuickieWebBot
{

Crawler::Crawler(unsigned int threadCount, QObject* parent)
	: QObject(parent)
	, m_stop(false)
{
	for (unsigned i = 0; i < threadCount; ++i)
	{
		QThread* thread = new QThread(this);
		m_workers[thread] = new CrawlerPageInfoAcceptor;
		m_workers[thread]->moveToThread(thread);

		VERIFY(connect(m_workers[thread], SIGNAL(pageParsed(QThread*, PageInfoPtr)),
			this, SLOT(onPageInfoAdded(QThread*, PageInfoPtr))));

		thread->start();
	}
}

void Crawler::start()
{
	m_stop = false;
	
	ServiceLocator* serviceLocator = ServiceLocator::instance();
	ModelController* modelController = serviceLocator->service<ModelController>();
	QUrl host = modelController->host();

	QMetaObject::invokeMethod(m_workers.begin()->second, "handlePage", Q_ARG(QUrl, host));
}

void Crawler::stop()
{
	m_stop = true;
}

void Crawler::setThreadCount(unsigned int threadCount) noexcept
{
	Q_UNUSED(threadCount);
}

void Crawler::onPageInfoAdded(QThread* fromThread, PageInfoPtr pageInfo)
{
	m_crawledUrlList.insert(pageInfo->url);

	CrawlerPageInfoAcceptor* pageInfoAcceptor = m_workers[fromThread];
	std::vector<QUrl> pageUrlList = pageInfoAcceptor->pageUrlList();

	auto findCrawledUrl = [&](const QUrl& url)
	{
		return m_crawledUrlList.find(url) != m_crawledUrlList.end();
	};

	pageUrlList.erase(std::remove_if(pageUrlList.begin(), pageUrlList.end(), findCrawledUrl), pageUrlList.end());
	m_internalUrlList.insert(pageUrlList.begin(), pageUrlList.end());

	QUrl url = *m_internalUrlList.begin();
	m_internalUrlList.erase(m_internalUrlList.begin());

	QMetaObject::invokeMethod(m_workers[fromThread], "handlePage", Q_ARG(QUrl, url));
}

}