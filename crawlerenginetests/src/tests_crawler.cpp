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

TestsCrawler::TestsCrawler(unsigned int threadCount, QObject* parent)
	: Crawler(threadCount, parent)
{
}

TestsCrawler::~TestsCrawler()
{
	m_sequencedDataCollectionThread->quit();
	m_sequencedDataCollectionThread->wait();
	m_sequencedDataCollectionThread->deleteLater();

	m_receiverThread->quit();
	m_receiverThread->wait();
	m_receiverThread->deleteLater();
}


void TestsCrawler::initialize()
{
	Crawler::initialize();

	m_receiver = std::make_unique<ParsedPageReceiver>(this, sequencedDataCollection());

	m_receiverThread = new Common::NamedThread("ParsedPageReceiverThread");
	m_receiverThread->start();

	m_receiver->moveToThread(m_receiverThread);

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

void TestsCrawler::waitForClearingDataDone(int seconds, const char* timeoutMessage) const
{
	std::future<void> future = m_receiver->getWaitForClearingData();

	if (future.wait_for(std::chrono::seconds(seconds)) == std::future_status::timeout)
	{
		throw TimeOutException(timeoutMessage);
	}
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

TestsDownloader* TestsCrawler::testDownloader() const
{
	return m_downloader;
}

void TestsCrawler::checkSequencedDataCollectionConsistency()
{
	std::vector<const ParsedPage*> crawledPages = m_receiver->storageItems(StorageType::CrawledUrlStorageType);
	std::vector<const ParsedPage*> blockedForIndexing = m_receiver->getLinksFromUnorderedDataCollection(StorageType::BlockedForSEIndexingStorageType);

	for (int i = static_cast<int>(StorageType::BeginEnumStorageType) + 1; i < static_cast<int>(StorageType::EndEnumStorageType); ++i)
	{
		std::vector<const ParsedPage*> storagePages = 
			m_receiver->storageItems(static_cast<StorageType>(i));

		for (const ParsedPage* page : storagePages)
		{
			const auto pageIt = std::find(std::begin(crawledPages), std::end(crawledPages), page);
			const auto pageBlocked = std::find(std::begin(blockedForIndexing), std::end(blockedForIndexing), page) != std::end(blockedForIndexing);

			const bool pageExists = pageIt != std::end(crawledPages);
			EXPECT_EQ(true, pageExists || page->resourceType == ResourceType::ResourceOther || pageBlocked);

			for (const ResourceLink& link : page->linksOnThisPage)
			{
				ParsedPage* resourcePage = link.resource.lock().get();

				if (!resourcePage || !PageParserHelpers::isHttpOrHttpsScheme(resourcePage->url))
				{
					continue;
				}

				const auto resourceAddressComparator = [resourcePage](const ParsedPage* page)
				{
					return resourcePage->url.canonizedUrlStr() == page->url.canonizedUrlStr();
				};

				const auto resourceIt = std::find_if(std::begin(crawledPages), std::end(crawledPages), resourceAddressComparator);
				const bool resourceExists = resourceIt != std::end(crawledPages);

				const bool isNofollowBlockedLink = link.linkParameter == LinkParameter::NofollowParameter &&
					(resourcePage->isThisExternalPage ? !options()->followExternalNofollow() : !options()->followInternalNofollow());

				const bool isSubdomainBlocked = !options()->checkSubdomains() &&
					PageParserHelpers::isSubdomain(options()->startCrawlingPage(), resourcePage->url);

				const bool isBlockedByOutsideFolderLink = !options()->crawlOutsideOfStartFolder() &&
					!PageParserHelpers::isUrlInsideBaseUrlFolder(options()->startCrawlingPage(), resourcePage->url);

				if (isNofollowBlockedLink || isSubdomainBlocked || isBlockedByOutsideFolderLink)
				{
					EXPECT_EQ(false, resourceExists);
				}
				else
				{
					if (ModelController::resourceShouldBeProcessed(resourcePage->resourceType, options()->data()))
					{
						if (page->storages[BlockedForSEIndexingStorageType])
						{
							// TODO: implement

						}
						else
						{
							EXPECT_EQ(true, resourceExists);
						}
					}
					else
					{
						// page can be added to dofollow/nofollow/etc storages 
						// event if resource of this type shouldn't be processed
						// (by the current implementation, maybe this is not a proper behaviour)
						EXPECT_EQ(true, page->storages[StorageType::PendingResourcesStorageType]);
					}
				}
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
}

IHostInfoProvider* TestsCrawler::createHostInfoProvider() const
{
	return new TestsHostInfoProvider;
}

IWebScreenShot* TestsCrawler::createWebScreenShot()
{
	return new TestsWebScreenShot(this);
}

void TestsCrawler::waitForCrawlingDone()
{
	m_receiver->wait();
}

}
