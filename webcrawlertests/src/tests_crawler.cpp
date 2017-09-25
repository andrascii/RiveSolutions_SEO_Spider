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
	, m_count(10000000)
{
	m_receiverThread = new QThread();
	moveToThread(m_receiverThread);
	m_receiverThread->start();

	QObject::connect(sequencedDataCollection, &WebCrawler::SequencedDataCollection::parsedPageAdded,
		this, &ParsedPageReceiver::onParsedPageAdded, Qt::QueuedConnection);
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
	// TODO: use storageType
	const WebCrawler::SequencedDataCollection::SequencedStorageTypePtr& storage = m_sequencedDataCollection->storage(static_cast<WebCrawler::StorageType>(storageType));
	m_parsedPages.push_back((*storage)[row]);
	if (static_cast<int>(m_parsedPages.size()) == m_count)
	{
		m_promise.set_value(m_parsedPages);
	}
}

std::future<std::vector<WebCrawler::ParsedPagePtr>> ParsedPageReceiver::getParsedPages(int count, int storageType)
{
	m_count = count;

	m_promise = std::promise<std::vector<WebCrawler::ParsedPagePtr>>();

	const WebCrawler::SequencedDataCollection::SequencedStorageTypePtr& storage = m_sequencedDataCollection->storage(static_cast<WebCrawler::StorageType>(storageType));
	int readyCount = storage->size();
	for (int row = 0; row < readyCount; ++row)
	{
		m_parsedPages.push_back((*storage)[row]);
	}

	if (static_cast<int>(m_parsedPages.size()) >= m_count)
	{
		m_promise.set_value(m_parsedPages);
	}

	return m_promise.get_future();
}

TestsCrawler::TestsCrawler(unsigned int threadCount)
	: WebCrawler::Crawler(threadCount, (m_sequensedCollectionThread = new QThread()))
{

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

	ParsedPageReceiver receiver(guiStorage());
	std::future<std::vector<WebCrawler::ParsedPagePtr>> future = receiver.getParsedPages(count, WebCrawler::CrawledUrlStorageType);

	if (future.wait_for(std::chrono::seconds(seconds)) == std::future_status::timeout)
	{
		throw TimeOutException("Timeout");
	}

	return future.get();
}

WebCrawler::IQueuedDownloader* TestsCrawler::createQueuedDownloader() const noexcept
{
	return new TestsQueudedDownoader();
}

}