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

	QObject::connect(sequencedDataCollection, &WebCrawler::SequencedDataCollection::parsedPageLinksToThisResourceChanged,
		this, &ParsedPageReceiver::onParsedPageLinksToThisResourceChanged, Qt::QueuedConnection);

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

void ParsedPageReceiver::onParsedPageLinksToThisResourceChanged(WebCrawler::LinksToThisResourceChanges changes)
{
	for (const WebCrawler::LinksToThisResourceChanges::Change& change : changes.changes)
	{
		m_linksToThisResourceChanges[change.page].push_back(changes);
	}
	
	checkLinksToThisResourceConditions(WebCrawler::ParsedPagePtr());
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

void ParsedPageReceiver::checkLinksToThisResourceConditions(WebCrawler::ParsedPagePtr page)
{
	for (auto it = m_linksToThisResourceConditions.begin(); it != m_linksToThisResourceConditions.end(); ++it)
	{
		if (!page || it->first == page)
		{
			if (static_cast<int>(m_linksToThisResourceChanges[page].size()) >= it->second.first)
			{
				it->second.second.set_value(m_linksToThisResourceChanges[page]);
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

std::vector<WebCrawler::ParsedPagePtr> ParsedPageReceiver::storageItems(WebCrawler::StorageType storage) const
{
	auto it = m_parsedPages.find(storage);

	return it != m_parsedPages.end() ? it->second : std::vector<WebCrawler::ParsedPagePtr>();
}

std::future<std::vector<WebCrawler::LinksToThisResourceChanges>> ParsedPageReceiver::getLinksToThisResourceChanges(WebCrawler::ParsedPagePtr page, int count)
{
	m_linksToThisResourceConditions[page] = std::make_pair(count, std::promise<std::vector<WebCrawler::LinksToThisResourceChanges>>());
	checkLinksToThisResourceConditions(page);
	return m_linksToThisResourceConditions[page].second.get_future();

}

TestsCrawler::TestsCrawler(unsigned int threadCount, const WebCrawler::CrawlerOptions& options)
	: WebCrawler::Crawler(threadCount, (m_sequensedCollectionThread = new QThread()))
	, m_testCrawlerOptions(options)
	, m_receiver(std::make_unique<ParsedPageReceiver>(sequencedDataCollection()))
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

std::vector<WebCrawler::ParsedPagePtr> TestsCrawler::waitForParsedPageReceived(WebCrawler::StorageType storage, int count, int seconds, const char* timeoutMessage) const
{
	ASSERT(m_crawlerThread != QThread::currentThread());
	ASSERT(m_crawlerThread->isRunning());
	std::future<std::vector<WebCrawler::ParsedPagePtr>> future = m_receiver->getParsedPages(count, storage);

	if (future.wait_for(std::chrono::seconds(seconds)) == std::future_status::timeout)
	{
		throw TimeOutException(timeoutMessage);
	}

	return future.get();
}

std::vector<WebCrawler::ParsedPagePtr> TestsCrawler::storageItems(WebCrawler::StorageType storage) const
{
	return m_receiver->storageItems(storage);
}

std::vector<WebCrawler::LinksToThisResourceChanges> TestsCrawler::waitForLinksToThisResourceChangesReceived(WebCrawler::ParsedPagePtr page, int count, int seconds) const
{
	ASSERT(m_crawlerThread != QThread::currentThread());
	ASSERT(m_crawlerThread->isRunning());
	std::future<std::vector<WebCrawler::LinksToThisResourceChanges>> future = m_receiver->getLinksToThisResourceChanges(page, count);

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

	QTimer::singleShot(50, [this] { m_cond(); });
}

WebCrawler::IQueuedDownloader* TestsCrawler::createQueuedDownloader() const noexcept
{
	return new TestsQueudedDownoader();
}

}