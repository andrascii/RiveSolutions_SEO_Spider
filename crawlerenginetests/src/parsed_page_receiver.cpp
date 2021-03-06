#include "stdafx.h"
#include "parsed_page_receiver.h"
#include "crawler_shared_state.h"
#include "unordered_data_collection.h"
#include "isequenced_storage.h"

namespace CrawlerEngineTests
{

ParsedPageReceiver::ParsedPageReceiver(const TestsCrawler* crawler, const SequencedDataCollection* sequencedDataCollection)
	: m_sequencedDataCollection(sequencedDataCollection)
	, m_allPagesReceived(false)
	, m_finished(false)
{
	VERIFY(connect(sequencedDataCollection, &SequencedDataCollection::parsedPageAdded,
		this, &ParsedPageReceiver::onParsedPageAdded, Qt::QueuedConnection));

	VERIFY(connect(sequencedDataCollection, &SequencedDataCollection::parsedPageRemoved,
		this, &ParsedPageReceiver::onParsedPageRemoved, Qt::QueuedConnection));

	VERIFY(connect(sequencedDataCollection, &SequencedDataCollection::parsedPagesRemoved,
		this, &ParsedPageReceiver::onParsedPagesRemoved, Qt::QueuedConnection));

	VERIFY(connect(sequencedDataCollection, &SequencedDataCollection::parsedPageLinksToThisResourceChanged,
		this, &ParsedPageReceiver::onParsedPageLinksToThisResourceChanged, Qt::QueuedConnection));

	VERIFY(connect(crawler, &Crawler::crawlerStarted,
		this, &ParsedPageReceiver::onCrawlerStarted, Qt::QueuedConnection));

	VERIFY(connect(crawler, &Crawler::crawlerFinished,
		this, &ParsedPageReceiver::onCrawlerFinished, Qt::QueuedConnection));

	VERIFY(connect(crawler, &Crawler::onAboutClearData,
		this, &ParsedPageReceiver::onAboutClearData, Qt::QueuedConnection));

	VERIFY(connect(crawler, &Crawler::serializationProcessDone,
		this, &ParsedPageReceiver::onSerializationDone, Qt::QueuedConnection));

	VERIFY(connect(crawler, &Crawler::deserializationProcessDone,
		this, &ParsedPageReceiver::onDeserializationDone, Qt::QueuedConnection));

	VERIFY(connect(crawler->sequencedDataCollection(), &SequencedDataCollection::endClearData,
		this, &ParsedPageReceiver::onDataCleared, Qt::QueuedConnection));

	VERIFY(connect(crawler->unorderedDataCollection(), SIGNAL(parsedPageAdded(CrawlerEngine::ParsedPagePtr, CrawlerEngine::StorageType, int)),
		this, SLOT(onUnorderedDataCollectionPageAdded(CrawlerEngine::ParsedPagePtr, CrawlerEngine::StorageType, int)), Qt::QueuedConnection));

	VERIFY(connect(crawler->unorderedDataCollection(), SIGNAL(parsedPageAdded(CrawlerEngine::WorkerResult, CrawlerEngine::StorageType)),
		this, SLOT(onUnorderedDataCollectionPageAdded(CrawlerEngine::WorkerResult, CrawlerEngine::StorageType)), Qt::QueuedConnection));

	VERIFY(connect(crawler->unorderedDataCollection(), SIGNAL(parsedPageRemoved(CrawlerEngine::ParsedPagePtr, CrawlerEngine::StorageType, int)),
		this, SLOT(onUnorderedDataCollectionPageRemoved(CrawlerEngine::ParsedPagePtr, CrawlerEngine::StorageType, int)), Qt::QueuedConnection));
}

void ParsedPageReceiver::onParsedPageAdded(int row, StorageType type)
{
	const ISequencedStorage& storage = *m_sequencedDataCollection->storage(type);

	m_parsedPages[type].push_back(storage[row]);

	checkWaitCondition(type);
	checkAllPagesReceived();
}

void ParsedPageReceiver::onParsedPageRemoved(int row, StorageType type)
{
	std::vector<const ParsedPage*>& destinationStorage = m_parsedPages[type];
	const auto removingItemIterator = destinationStorage.begin() + row;
	destinationStorage.erase(removingItemIterator);

	checkWaitCondition(type);
}

void ParsedPageReceiver::onParsedPagesRemoved(int count, StorageType type)
{
	Q_UNUSED(count);

	const ISequencedStorage& storage = *m_sequencedDataCollection->storage(type);
	std::vector<const ParsedPage*>& data = m_parsedPages[type];

	data.clear();

	for (int i = 0; i < storage.size(); ++i)
	{
		data.push_back(storage[i]);
	}
}

void ParsedPageReceiver::onParsedPageLinksToThisResourceChanged(LinksToThisResourceChanges changes)
{
	for (const LinksToThisResourceChanges::Change& change : changes.changes)
	{
		m_linksToThisResourceChanges[change.page.lock().get()].push_back(changes);
	}

	checkLinksToThisResourceConditions(nullptr);
}

void ParsedPageReceiver::onCrawlerStarted()
{
	m_waitForClearingData = std::promise<void>();
	m_finished = false;
}

void ParsedPageReceiver::onCrawlerFinished()
{
	m_finished = true;
	checkAllPagesReceived();
}

void ParsedPageReceiver::onAboutClearData()
{
	m_waitConditions.clear();
	m_parsedPages.clear();

	m_linksToThisResourceChanges.clear();
	m_linksToThisResourceConditions.clear();

	m_allPagesReceivedPromise = std::promise<std::vector<const ParsedPage*>>();
	m_allPagesReceived = false;
}

void ParsedPageReceiver::onDataCleared()
{
	try
	{
		m_waitForClearingData.set_value();
	}
	catch (const std::future_error& error)
	{
		if (error.code() != std::make_error_condition(std::future_errc::promise_already_satisfied))
		{
			throw;
		}
	}
}

void ParsedPageReceiver::onSerializationDone()
{
	//QThread::currentThread()->msleep(1000);
	m_waitForSerializationDone.set_value();
}

void ParsedPageReceiver::onDeserializationDone()
{
	//QThread::currentThread()->msleep(1000);
	m_waitForDeserializationDone.set_value();
}

void ParsedPageReceiver::onUnorderedDataCollectionPageAdded(ParsedPagePtr page, StorageType type, int turnaround)
{
	Q_UNUSED(turnaround);
	std::lock_guard<std::mutex> locker(m_ucMutex);

	m_unorderedDataCollectionPages[type].push_back(page.get());
}

void ParsedPageReceiver::onUnorderedDataCollectionPageAdded(WorkerResult result, StorageType type)
{
	onUnorderedDataCollectionPageAdded(result.incomingPage(), type, result.turnaround());
}

void ParsedPageReceiver::onUnorderedDataCollectionPageRemoved(ParsedPagePtr page, StorageType type, int turnaround)
{
	Q_UNUSED(turnaround);
	std::lock_guard<std::mutex> locker(m_ucMutex);

	auto it = std::find_if(std::begin(m_unorderedDataCollectionPages[type]), std::end(m_unorderedDataCollectionPages[type]),
		[pagePointer = page.get()](const ParsedPage* item) { return item == pagePointer; });

	ASSERT(it != std::end(m_unorderedDataCollectionPages[type]));

	m_unorderedDataCollectionPages[type].erase(it);
}

std::vector<const ParsedPage*> ParsedPageReceiver::getLinksFromUnorderedDataCollection(StorageType type) const
{
	std::lock_guard<std::mutex> locker(m_ucMutex);

	ASSERT(m_allPagesReceived.load());

	auto iterator = m_unorderedDataCollectionPages.find(type);

	return iterator != m_unorderedDataCollectionPages.end() ? iterator->second : std::vector<const ParsedPage*>();
}

std::future<void> ParsedPageReceiver::getWaitForClearingData()
{
	return m_waitForClearingData.get_future();
}

std::future<void> ParsedPageReceiver::getWaitForSerializationDone()
{
	return m_waitForSerializationDone.get_future();
}

std::future<void> ParsedPageReceiver::getWaitForDeserializationDone()
{
	return m_waitForDeserializationDone.get_future();
}

void ParsedPageReceiver::wait()
{
	std::future<std::vector<const ParsedPage*>> future = getAllCrawledPages();

	if (!future.valid())
	{
		return;
	}

	future.wait();
}

void ParsedPageReceiver::clearReceivedData()
{
	if (thread() == QThread::currentThread())
	{
		onAboutClearData();
	}
	else
	{
		VERIFY(QMetaObject::invokeMethod(this, "onAboutClearData", Qt::BlockingQueuedConnection));
	}
}

void ParsedPageReceiver::checkWaitCondition(StorageType storageType)
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

void ParsedPageReceiver::checkLinksToThisResourceConditions(const ParsedPage* page)
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

void ParsedPageReceiver::checkAllPagesReceived()
{
	INFOLOG << "Started";
	if (!m_allPagesReceived &&
		m_finished &&
		m_parsedPages[CrawledUrlStorageType].size() == static_cast<size_t>(CrawlerSharedState::instance()->sequencedDataCollectionLinksCount()))
	{
		INFOLOG << CrawlerSharedState::instance()->sequencedDataCollectionLinksCount() << m_parsedPages[StorageType::CrawledUrlStorageType].size();
		m_allPagesReceivedPromise.set_value(m_parsedPages[StorageType::CrawledUrlStorageType]);
		m_allPagesReceived = true;
	}

	INFOLOG << "Ended";
}

std::future<std::vector<const ParsedPage*>> ParsedPageReceiver::getParsedPages(int count, StorageType storageType)
{
	m_waitConditions[storageType] = std::make_pair(count, std::promise<std::vector<const ParsedPage*>>());
	checkWaitCondition(storageType);
	return m_waitConditions[storageType].second.get_future();
}

std::future<std::vector<const ParsedPage*>> ParsedPageReceiver::getAllCrawledPages()
{
	return m_allPagesReceivedPromise.get_future();
}

std::vector<const ParsedPage*> ParsedPageReceiver::storageItems(StorageType storage) const
{
	auto it = m_parsedPages.find(storage);

	return it != m_parsedPages.end() ? it->second : std::vector<const ParsedPage*>();
}

std::future<std::vector<LinksToThisResourceChanges>> ParsedPageReceiver::getLinksToThisResourceChanges(const ParsedPage* page, int count)
{
	m_linksToThisResourceConditions[page] = std::make_pair(count, std::promise<std::vector<LinksToThisResourceChanges>>());
	checkLinksToThisResourceConditions(page);
	return m_linksToThisResourceConditions[page].second.get_future();
}

}