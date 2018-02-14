#include "tests_crawler.h"
#include "tests_downloader.h"
#include "page_parser_helpers.h"
#include "test_sequenced_data_collection.h"
#include "unordered_data_collection.h"
#include "model_controller.h"
#include "tests_web_screenshot.h"
#include "tests_host_info_provider.h"

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

void TestsCrawler::waitForSerializationDone(int seconds, const char* timeoutMessage) const
{
	QDateTime elapsed = QDateTime::currentDateTime();
	elapsed = elapsed.addSecs(seconds);

	while (QDateTime::currentDateTime() < elapsed)
	{
		if (state() != StateSerializaton)
		{
			return;
		}
		
		QApplication::instance()->processEvents();
	}

	throw TimeOutException(timeoutMessage);
}

void TestsCrawler::waitForDeserializationDone(int seconds, const char* timeoutMessage) const
{
	QDateTime elapsed = QDateTime::currentDateTime();
	elapsed = elapsed.addSecs(seconds);

	while (QDateTime::currentDateTime() < elapsed)
	{
		if (state() != StateDeserializaton)
		{
			return;
		}

		QApplication::instance()->processEvents();
	}

	throw TimeOutException(timeoutMessage);
}

void TestsCrawler::waitForRefreshPageDone(int seconds, const char* timeoutMessage) const
{
	QDateTime elapsed = QDateTime::currentDateTime();
	elapsed = elapsed.addSecs(seconds);

	while (QDateTime::currentDateTime() < elapsed)
	{
		if (state() != StatePageRefresh)
		{
			return;
		}

		QApplication::instance()->processEvents();
	}

	throw TimeOutException(timeoutMessage);
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

void TestsCrawler::clearReceivedData()
{
	m_receiver->clearReceivedData();
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
			EXPECT_EQ(true, pageExists || page->resourceType == ResourceType::ResourceOther);

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

void TestsCrawler::initSequencedDataCollection()
{
	m_sequencedDataCollectionThread = new QThread;
	m_sequencedDataCollection = std::make_unique<TestSequencedDataCollection>(m_modelController->data());
	m_sequencedDataCollection->initialize();
	m_sequencedDataCollection->moveToThread(m_sequencedDataCollectionThread);

	VERIFY(connect(m_sequencedDataCollection.get(), &SequencedDataCollection::refreshPageDone,
		this, &TestsCrawler::onRefreshPageDone, Qt::QueuedConnection));
}

IHostInfoProvider* TestsCrawler::createHostInfoProvider() const
{
	return new TestsHostInfoProvider;
}

IWebScreenShot* TestsCrawler::createWebScreenShot()
{
	return new TestsWebScreenShot(this);
}

}
