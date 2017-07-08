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
	qRegisterMetaType<PageInfoPtr>();

	for (unsigned i = 0; i < threadCount; ++i)
	{
		QThread* thread = new QThread(this);
		m_workers[thread] = new CrawlerPageInfoAcceptor;
		m_workers[thread]->moveToThread(thread);

		VERIFY(connect(m_workers[thread], SIGNAL(pageParsed(QThread*, PageInfoPtr)),
			this, SLOT(onPageInfoParsed(QThread*, PageInfoPtr)), Qt::QueuedConnection));

		thread->start();
	}
}

void Crawler::start()
{
	m_stop = false;
	m_initialCrawling = true;

	QUrl host = ServiceLocator::instance()->service<ModelController>()->host();
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

void Crawler::onPageInfoParsed(QThread* fromThread, PageInfoPtr pageInfo)
{
	ModelController* modelController = ServiceLocator::instance()->service<ModelController>();

	modelController->addPageInfo(pageInfo);
	m_crawledUrlList.insert(pageInfo->url);

	if (m_stop)
	{
		return;
	}

	CrawlerPageInfoAcceptor* pageInfoAcceptor = m_workers[fromThread];
	std::vector<QUrl> pageUrlList = pageInfoAcceptor->pageUrlList();

	auto findCrawledUrl = [&](const QUrl& url)
	{
		return m_crawledUrlList.find(url) != m_crawledUrlList.end();
	};

	pageUrlList.erase(std::remove_if(pageUrlList.begin(), pageUrlList.end(), findCrawledUrl), pageUrlList.end());
	m_internalUrlList.insert(pageUrlList.begin(), pageUrlList.end());

	if (m_internalUrlList.size())
	{
		QUrl url = *m_internalUrlList.begin();
		m_internalUrlList.erase(m_internalUrlList.begin());

		//
		// if we have url count greater then all threads except fromThread
		// or equal then start all of these threads for crawling too
		//
		if (m_initialCrawling && m_internalUrlList.size() >= m_workers.size() - 1)
		{
			for(const std::pair<const QThread*, CrawlerPageInfoAcceptor*>& pair : m_workers)
			{
				QUrl url = *m_internalUrlList.begin();
				m_internalUrlList.erase(m_internalUrlList.begin());

				QMetaObject::invokeMethod(pair.second, "handlePage", Q_ARG(QUrl, url));
			}

			m_initialCrawling = false;
		}

		QMetaObject::invokeMethod(m_workers[fromThread], "handlePage", Q_ARG(QUrl, url));
	}
}

}