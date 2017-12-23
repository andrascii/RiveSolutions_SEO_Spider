#include "tests_crawler.h"
#include "tests_downloader.h"
#include "page_parser_helpers.h"
#include "test_sequenced_data_collection.h"
#include "unordered_data_collection.h"
#include "model_controller.h"

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

TestsCrawler::TestsCrawler(unsigned int threadCount, const CrawlerOptions& options, QObject* parent)
	: Crawler(threadCount, parent)
	, m_testCrawlerOptions(options)
{
}

TestsCrawler::~TestsCrawler()
{
	m_sequencedDataCollectionThread->quit();
	m_sequencedDataCollectionThread->wait();
	m_sequencedDataCollectionThread->deleteLater();
}


void TestsCrawler::initialize()
{
	Crawler::initialize();

	m_sequencedDataCollectionThread = new QThread;

	createSequencedDataCollection(m_sequencedDataCollectionThread);
	m_receiver = std::make_unique<ParsedPageReceiver>(this, sequencedDataCollection());

	m_sequencedDataCollectionThread->start();
}

std::vector<const ParsedPage*> TestsCrawler::waitForParsedPageReceived(
	StorageType storage, int count, int seconds, const char* timeoutMessage) const
{
	std::future<std::vector<const ParsedPage*>> future = m_receiver->getParsedPages(count, storage);

	if (future.wait_for(std::chrono::seconds(seconds)) == std::future_status::timeout)
	{
		throw TimeOutException(timeoutMessage);
	}

	return future.get();
}

std::vector<const ParsedPage*> TestsCrawler::waitForAllCrawledPageReceived(int seconds, const char* timeoutMessage) const
{
	std::future<std::vector<const ParsedPage*>> future = m_receiver->getAllCrawledPages();

	if (future.wait_for(std::chrono::seconds(seconds)) == std::future_status::timeout)
	{
		throw TimeOutException(timeoutMessage);
	}

	return future.get();
}

std::vector<const ParsedPage*> TestsCrawler::storageItems(StorageType storage) const
{
	return m_receiver->storageItems(storage);
}

std::vector<LinksToThisResourceChanges> TestsCrawler::waitForLinksToThisResourceChangesReceived(const ParsedPage* page, int count, int seconds) const
{
	std::future<std::vector<LinksToThisResourceChanges>> future = 
		m_receiver->getLinksToThisResourceChanges(page, count);

	if (future.wait_for(std::chrono::seconds(seconds)) == std::future_status::timeout)
	{
		throw TimeOutException("Timeout");
	}

	return future.get();
}

std::vector<const ParsedPage*> TestsCrawler::getLinksFromUnorderedDataCollection(StorageType type) const
{
	return m_receiver->getLinksFromUnorderedDataCollection(type);
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
	std::vector<const ParsedPage*> crawledPages = m_receiver->storageItems(StorageType::CrawledUrlStorageType);

	for (int i = static_cast<int>(StorageType::BeginEnumStorageType) + 1; i < static_cast<int>(StorageType::EndEnumStorageType); ++i)
	{
		std::vector<const ParsedPage*> storagePages = 
			m_receiver->storageItems(static_cast<StorageType>(i));

		for (const ParsedPage* page : storagePages)
		{
			auto pageIt = std::find(std::begin(crawledPages), std::end(crawledPages), page);

			const bool pageExists = pageIt != std::end(crawledPages);
			EXPECT_EQ(true, pageExists);

			for (const ResourceLink& link : page->linksOnThisPage)
			{
				ParsedPage* resourcePage = link.resource.lock().get();

				if (!resourcePage || !PageParserHelpers::isHttpOrHttpsScheme(resourcePage->url))
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

const UnorderedDataCollection* TestsCrawler::unorderedDataCollection() const
{
	return m_modelController->data();
}

IDownloader* TestsCrawler::createDownloader() const
{
	m_downloader = new TestsDownloader;
	return m_downloader;
}

void TestsCrawler::createSequencedDataCollection(QThread* targetThread) const
{
	m_sequencedDataCollection.reset(new TestSequencedDataCollection);

	QThread* mainThread = QApplication::instance() != nullptr ? QApplication::instance()->thread() : nullptr;
	targetThread = targetThread != nullptr ? targetThread : mainThread;

	ASSERT(targetThread != nullptr);

	m_sequencedDataCollection->moveToThread(targetThread);

	VERIFY(connect(m_modelController->data(), &UnorderedDataCollection::parsedPageAdded, m_sequencedDataCollection.get(),
		&TestSequencedDataCollection::addParsedPage, Qt::QueuedConnection));

	VERIFY(connect(m_modelController->data(), &UnorderedDataCollection::parsedPageLinksToThisResourceChanged, m_sequencedDataCollection.get(),
		&TestSequencedDataCollection::parsedPageLinksToThisResourceChanged, Qt::QueuedConnection));

	VERIFY(connect(m_modelController->data(), &UnorderedDataCollection::dataCleared, m_sequencedDataCollection.get(),
		&TestSequencedDataCollection::onDataCleared, Qt::QueuedConnection));

	static_cast<TestSequencedDataCollection*>(m_sequencedDataCollection.get())->initializeStorages();
}

}
