#include "tests_crawler.h"
#include "tests_downloader.h"
#include "tests_robots_txt_loader.h"
#include "page_parser_helpers.h"

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

ParsedPageReceiver::ParsedPageReceiver(const CrawlerEngine::SequencedDataCollection* sequencedDataCollection)
	: m_sequencedDataCollection(sequencedDataCollection)
{
	m_receiverThread = new Common::NamedThread("ParsedPageReceiverThread");
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

void ParsedPageReceiver::onParsedPageAdded(int row, int type)
{
	CrawlerEngine::StorageType storageType = static_cast<CrawlerEngine::StorageType>(type);

	const CrawlerEngine::SequencedStorage& storage = *m_sequencedDataCollection->storage(storageType);

	m_parsedPages[storageType].push_back(storage[row]);

	checkWaitCondition(storageType);
}

void ParsedPageReceiver::onParsedPageLinksToThisResourceChanged(CrawlerEngine::LinksToThisResourceChanges changes)
{
	for (const CrawlerEngine::LinksToThisResourceChanges::Change& change : changes.changes)
	{
		m_linksToThisResourceChanges[change.page.get()].push_back(changes);
	}
	
	checkLinksToThisResourceConditions(nullptr);
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

void ParsedPageReceiver::checkLinksToThisResourceConditions(const CrawlerEngine::ParsedPage* page)
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

std::future<std::vector<const CrawlerEngine::ParsedPage*>> ParsedPageReceiver::getParsedPages(int count, int storageType)
{
	m_waitConditions[storageType] = std::make_pair(count, std::promise<std::vector<const CrawlerEngine::ParsedPage*>>());
	checkWaitCondition(storageType);
	return m_waitConditions[storageType].second.get_future();
}

std::vector<const CrawlerEngine::ParsedPage*> ParsedPageReceiver::storageItems(CrawlerEngine::StorageType storage) const
{
	auto it = m_parsedPages.find(storage);

	return it != m_parsedPages.end() ? it->second : std::vector<const CrawlerEngine::ParsedPage*>();
}

std::future<std::vector<CrawlerEngine::LinksToThisResourceChanges>> ParsedPageReceiver::getLinksToThisResourceChanges(const CrawlerEngine::ParsedPage* page, int count)
{
	m_linksToThisResourceConditions[page] = std::make_pair(count, std::promise<std::vector<CrawlerEngine::LinksToThisResourceChanges>>());
	checkLinksToThisResourceConditions(page);
	return m_linksToThisResourceConditions[page].second.get_future();
}

TestsCrawler::TestsCrawler(unsigned int threadCount, const CrawlerEngine::CrawlerOptions& options, QObject* parent)
	: CrawlerEngine::Crawler(threadCount, parent)
	, m_testCrawlerOptions(options)
{
	initialize();

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

std::vector<const CrawlerEngine::ParsedPage*> TestsCrawler::waitForParsedPageReceived(CrawlerEngine::StorageType storage, int count, int seconds, const char* timeoutMessage) const
{
	std::future<std::vector<const CrawlerEngine::ParsedPage*>> future = m_receiver->getParsedPages(count, storage);

	if (future.wait_for(std::chrono::seconds(seconds)) == std::future_status::timeout)
	{
		throw TimeOutException(timeoutMessage);
	}

	return future.get();
}

std::vector<const CrawlerEngine::ParsedPage*> TestsCrawler::storageItems(CrawlerEngine::StorageType storage) const
{
	return m_receiver->storageItems(storage);
}

std::vector<CrawlerEngine::LinksToThisResourceChanges> TestsCrawler::waitForLinksToThisResourceChangesReceived(const CrawlerEngine::ParsedPage* page, int count, int seconds) const
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

TestsDownloader* TestsCrawler::testDownloader() const
{
	return m_downloader;
}

void TestsCrawler::checkSequencedDataCollectionConsistency()
{
	std::vector<const CrawlerEngine::ParsedPage*> crawledPages = m_receiver->storageItems(CrawlerEngine::CrawledUrlStorageType);

	for (int i = CrawlerEngine::BeginEnumStorageType + 1; i < CrawlerEngine::EndEnumStorageType; ++i)
	{
		std::vector<const CrawlerEngine::ParsedPage*> storagePages = 
			m_receiver->storageItems(static_cast<CrawlerEngine::StorageType>(i));

		for (const CrawlerEngine::ParsedPage* page : storagePages)
		{
			auto pageIt = std::find(std::begin(crawledPages), std::end(crawledPages), page);

			const bool pageExists = pageIt != std::end(crawledPages);
			EXPECT_EQ(true, pageExists);

			for (const CrawlerEngine::ResourceLink& resource : page->linksOnThisPage)
			{
				CrawlerEngine::ParsedPage* resourcePage = resource.resource.lock().get();

				if (!CrawlerEngine::PageParserHelpers::isHttpOrHttpsScheme(resourcePage->url))
				{
					continue;
				}

				auto resourceIt = std::find(std::begin(crawledPages), std::end(crawledPages), resourcePage);

				const bool resourceExists = resourceIt != std::end(crawledPages);
				EXPECT_EQ(true, resourceExists);
			}
		}
	}
}

CrawlerEngine::IDownloader* TestsCrawler::createDownloader() const
{
	m_downloader = new TestsDownloader();
	return m_downloader;
}

CrawlerEngine::IRobotsTxtLoader* TestsCrawler::createRobotsTxtLoader() const
{
	return new TestsRobotsTxtLoader();
}

}
