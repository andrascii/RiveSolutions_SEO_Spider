#include "tests_crawler.h"
#include "tests_queued_downloader.h"

namespace WebCrawlerTests
{

class TimeOutException: public std::exception
{
public:
	TimeOutException(const char* message)
		: std::exception(message)
	{
		
	}
};

ParsedPageReceiver::ParsedPageReceiver(WebCrawler::SequencedDataCollection* sequencedDataCollection)
	: m_sequencedDataCollection(sequencedDataCollection)
{
	m_receiverThread = new QThread();
	moveToThread(m_receiverThread);
	QObject::connect(sequencedDataCollection, &WebCrawler::SequencedDataCollection::parsedPageAdded,
		this, &ParsedPageReceiver::onParsedPageAdded, Qt::QueuedConnection);


	m_receiverThread->start();
}

ParsedPageReceiver::~ParsedPageReceiver()
{
	// Dtor should be called from a different thread
	ASSERT(thread() != QThread::currentThread());
	m_receiverThread->quit();
	m_receiverThread->wait();
	m_receiverThread->deleteLater();
}

void ParsedPageReceiver::onParsedPageAdded(int row, int storageType)
{
	const WebCrawler::SequencedDataCollection::SequencedStorageTypePtr& storage = m_sequencedDataCollection->storage(static_cast<WebCrawler::StorageType>(storageType));
	m_parsedPages[storageType].push_back((*storage)[row]);

	checkWaitCondition(storageType);
}

void ParsedPageReceiver::checkWaitCondition(int storageType)
{
	for (auto it = m_waitConditions.begin(); it != m_waitConditions.end(); ++it)
	{
		if (it->first == storageType)
		{
			if (static_cast<int>(m_parsedPages[storageType].size()) >= it->second.first)
			{
				it->second.second.set_value(m_parsedPages[storageType]);
			}
			break;
		}
	}
}

std::future<std::vector<WebCrawler::ParsedPagePtr>> ParsedPageReceiver::getParsedPages(int count, int storageType)
{
	m_waitConditions[storageType] = std::make_pair(count, std::promise<std::vector<WebCrawler::ParsedPagePtr>>());
	checkWaitCondition(storageType);
	return m_waitConditions[storageType].second.get_future();
}

TestsCrawler::TestsCrawler(unsigned int threadCount, const WebCrawler::CrawlerOptions& options)
	: WebCrawler::Crawler(threadCount, (m_sequensedCollectionThread = new QThread()))
	, m_testCrawlerOptions(options)
	, m_receiver(std::make_unique<ParsedPageReceiver>(guiStorage()))
{
	m_sequensedCollectionThread->start();
	
}

TestsCrawler::~TestsCrawler()
{
	// Dtor should be called from a different thread
	ASSERT(thread() != QThread::currentThread());
	m_sequensedCollectionThread->quit();
	m_sequensedCollectionThread->wait();
	m_sequensedCollectionThread->deleteLater();
}

std::vector<WebCrawler::ParsedPagePtr> TestsCrawler::waitForParsedPageReceived(int count, int seconds) const
{
	ASSERT(m_crawlerThread != QThread::currentThread());
	ASSERT(m_crawlerThread->isRunning());
	std::future<std::vector<WebCrawler::ParsedPagePtr>> future = m_receiver->getParsedPages(count, WebCrawler::CrawledUrlStorageType);

	if (future.wait_for(std::chrono::seconds(seconds)) == std::future_status::timeout)
	{
		throw TimeOutException("Timeout");
	}

	return future.get();
}

void TestsCrawler::startTestCrawler()
{
	startCrawling(m_testCrawlerOptions);
}

void TestsCrawler::setCondition(std::function<void()> cond)
{
	m_cond = cond;

	QTimer::singleShot(1000, [this] { m_cond(); });
}

WebCrawler::IQueuedDownloader* TestsCrawler::createQueuedDownloader() const noexcept
{
	return new TestsQueudedDownoader();
}

}