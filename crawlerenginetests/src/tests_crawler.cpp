#include "tests_crawler.h"
#include "tests_downloader.h"
#include "tests_robots_txt_loader.h"
#include "page_parser_helpers.h"
#include "handler_registry.h"
#include "test_sequenced_data_collection.h"
#include "unordered_data_collection.h"
#include "model_controller.h"

namespace CrawlerEngineTests
{

std::mutex TestsCrawler::s_mutex;

class TimeOutException: public std::exception
{
public:
	TimeOutException(const char* message)
		: std::exception(message)
	{
		
	}
};

TestsCrawler::TestsCrawler(unsigned int threadCount, const CrawlerEngine::CrawlerOptions& options, QObject* parent)
	: CrawlerEngine::Crawler(threadCount, parent)
	, m_testCrawlerOptions(options)
{
	initialize();

	m_sequencedDataCollectionThread = new QThread;

	createSequencedDataCollection(m_sequencedDataCollectionThread);
	m_receiver = std::make_unique<ParsedPageReceiver>(this, sequencedDataCollection());

	m_sequencedDataCollectionThread->start();
}

TestsCrawler::~TestsCrawler()
{
	m_sequencedDataCollectionThread->quit();
	m_sequencedDataCollectionThread->wait();
	m_sequencedDataCollectionThread->deleteLater();
}

std::vector<const CrawlerEngine::ParsedPage*> TestsCrawler::waitForParsedPageReceived(
	CrawlerEngine::StorageType storage, int count, int seconds, const char* timeoutMessage) const
{
	std::future<std::vector<const CrawlerEngine::ParsedPage*>> future = m_receiver->getParsedPages(count, storage);

	if (future.wait_for(std::chrono::seconds(seconds)) == std::future_status::timeout)
	{
		throw TimeOutException(timeoutMessage);
	}

	return future.get();
}

std::vector<const CrawlerEngine::ParsedPage*> TestsCrawler::waitForAllCrawledPageReceived(int seconds, const char* timeoutMessage) const
{
	std::future<std::vector<const CrawlerEngine::ParsedPage*>> future = m_receiver->getAllCrawledPages();

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

std::vector<CrawlerEngine::LinksToThisResourceChanges> TestsCrawler::waitForLinksToThisResourceChangesReceived(
	const CrawlerEngine::ParsedPage* page, int count, int seconds) const
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

void TestsCrawler::createSequencedDataCollection(QThread* targetThread) const
{
	m_sequencedDataCollection.reset(new TestSequencedDataCollection);

	QThread* mainThread = QApplication::instance() != nullptr ? QApplication::instance()->thread() : nullptr;
	targetThread = targetThread != nullptr ? targetThread : mainThread;

	ASSERT(targetThread != nullptr);

	m_sequencedDataCollection->moveToThread(targetThread);

	VERIFY(connect(m_modelController->data(), &CrawlerEngine::UnorderedDataCollection::parsedPageAdded, m_sequencedDataCollection.get(),
		&TestSequencedDataCollection::addParsedPage, Qt::QueuedConnection));

	VERIFY(connect(m_modelController->data(), &CrawlerEngine::UnorderedDataCollection::parsedPageLinksToThisResourceChanged, m_sequencedDataCollection.get(),
		&TestSequencedDataCollection::parsedPageLinksToThisResourceChanged, Qt::QueuedConnection));

	VERIFY(connect(m_modelController->data(), &CrawlerEngine::UnorderedDataCollection::dataCleared, m_sequencedDataCollection.get(),
		&TestSequencedDataCollection::onDataCleared, Qt::QueuedConnection));

	static_cast<TestSequencedDataCollection*>(m_sequencedDataCollection.get())->initializeStorages();
}

}
