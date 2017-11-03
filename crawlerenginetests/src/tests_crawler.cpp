#include "tests_crawler.h"
#include "tests_queued_downloader.h"
#include "tests_robots_txt_loader.h"

namespace CrawlerEngineTests
{

class TimeOutException: public std::exception
{
public:
	TimeOutException(const char* message)
		: std::exception(message)
	{
		
	}
};

ParsedPageReceiver::ParsedPageReceiver(CrawlerEngine::SequencedDataCollection* sequencedDataCollection)
	: m_sequencedDataCollection(sequencedDataCollection)
{
	m_receiverThread = new QThread();
	moveToThread(m_receiverThread);
	
	VERIFY(connect(sequencedDataCollection, &CrawlerEngine::SequencedDataCollection::parsedPageAdded,
		this, &ParsedPageReceiver::onParsedPageAdded, Qt::QueuedConnection));

	VERIFY(connect(sequencedDataCollection, &CrawlerEngine::SequencedDataCollection::parsedPageLinksToThisResourceChanged,
		this, &ParsedPageReceiver::onParsedPageLinksToThisResourceChanged, Qt::QueuedConnection));

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
	const CrawlerEngine::SequencedDataCollection::SequencedStorageTypePtr& storage = m_sequencedDataCollection->storage(static_cast<CrawlerEngine::StorageType>(storageType));
	m_parsedPages[storageType].push_back((*storage)[row]);

	checkWaitCondition(storageType);
}

void ParsedPageReceiver::onParsedPageLinksToThisResourceChanged(CrawlerEngine::LinksToThisResourceChanges changes)
{
	for (const CrawlerEngine::LinksToThisResourceChanges::Change& change : changes.changes)
	{
		m_linksToThisResourceChanges[change.page].push_back(changes);
	}
	
	checkLinksToThisResourceConditions(CrawlerEngine::ParsedPagePtr());
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

void ParsedPageReceiver::checkLinksToThisResourceConditions(CrawlerEngine::ParsedPagePtr page)
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

std::future<std::vector<CrawlerEngine::ParsedPagePtr>> ParsedPageReceiver::getParsedPages(int count, int storageType)
{
	m_waitConditions[storageType] = std::make_pair(count, std::promise<std::vector<CrawlerEngine::ParsedPagePtr>>());
	checkWaitCondition(storageType);
	return m_waitConditions[storageType].second.get_future();
}

std::vector<CrawlerEngine::ParsedPagePtr> ParsedPageReceiver::storageItems(CrawlerEngine::StorageType storage) const
{
	auto it = m_parsedPages.find(storage);

	return it != m_parsedPages.end() ? it->second : std::vector<CrawlerEngine::ParsedPagePtr>();
}

std::future<std::vector<CrawlerEngine::LinksToThisResourceChanges>> ParsedPageReceiver::getLinksToThisResourceChanges(CrawlerEngine::ParsedPagePtr page, int count)
{
	m_linksToThisResourceConditions[page] = std::make_pair(count, std::promise<std::vector<CrawlerEngine::LinksToThisResourceChanges>>());
	checkLinksToThisResourceConditions(page);
	return m_linksToThisResourceConditions[page].second.get_future();
}

TestsCrawler::TestsCrawler(unsigned int threadCount, const CrawlerEngine::CrawlerOptions& options, QObject* parent)
	: CrawlerEngine::Crawler(threadCount, parent)
	, m_testCrawlerOptions(options)
{
	m_sequencedDataCollectionThread = new QThread;

	createSequencedDataCollection(m_sequencedDataCollectionThread);
	m_receiver = std::make_unique<ParsedPageReceiver>(sequencedDataCollection());

	m_sequencedDataCollectionThread->start();
}

TestsCrawler::~TestsCrawler()
{
	m_sequencedDataCollectionThread->quit();
	m_sequencedDataCollectionThread->wait();
	m_sequencedDataCollectionThread->deleteLater();
}

std::vector<CrawlerEngine::ParsedPagePtr> TestsCrawler::waitForParsedPageReceived(CrawlerEngine::StorageType storage, int count, int seconds, const char* timeoutMessage) const
{
	std::future<std::vector<CrawlerEngine::ParsedPagePtr>> future = 
		m_receiver->getParsedPages(count, storage);

	if (future.wait_for(std::chrono::seconds(seconds)) == std::future_status::timeout)
	{
		throw TimeOutException(timeoutMessage);
	}

	return future.get();
}

std::vector<CrawlerEngine::ParsedPagePtr> TestsCrawler::storageItems(CrawlerEngine::StorageType storage) const
{
	return m_receiver->storageItems(storage);
}

std::vector<CrawlerEngine::LinksToThisResourceChanges> TestsCrawler::waitForLinksToThisResourceChangesReceived(CrawlerEngine::ParsedPagePtr page, int count, int seconds) const
{
	std::future<std::vector<CrawlerEngine::LinksToThisResourceChanges>> future = 
		m_receiver->getLinksToThisResourceChanges(page, count);

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

CrawlerEngine::IQueuedDownloader* TestsCrawler::createQueuedDownloader() const
{
	return new TestsQueudedDownoader();
}

CrawlerEngine::IRobotsTxtLoader* TestsCrawler::createRobotsTxtLoader() const
{
	return new TestsRobotsTxtLoader();
}

}
