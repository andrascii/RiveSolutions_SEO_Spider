#include "parsed_page_receiver.h"

namespace CrawlerEngineTests
{

ParsedPageReceiver::ParsedPageReceiver(const TestsCrawler* crawler, const SequencedDataCollection* sequencedDataCollection)
	: m_sequencedDataCollection(sequencedDataCollection)
	, m_allPagesReceived(false)
{
	m_receiverThread = new Common::NamedThread("ParsedPageReceiverThread");
	moveToThread(m_receiverThread);

	VERIFY(connect(sequencedDataCollection, &SequencedDataCollection::parsedPageAdded,
		this, &ParsedPageReceiver::onParsedPageAdded, Qt::QueuedConnection));

	VERIFY(connect(sequencedDataCollection, &SequencedDataCollection::parsedPageLinksToThisResourceChanged,
		this, &ParsedPageReceiver::onParsedPageLinksToThisResourceChanged, Qt::QueuedConnection));

	VERIFY(connect(crawler, &Crawler::crawlingProgress,
		this, &ParsedPageReceiver::onCrawlingProgress, Qt::QueuedConnection));

// 	VERIFY(connect(crawler->unorderedDataCollection(), SIGNAL(parsedPageAdded(ParsedPagePtr, int)), 
// 		this, SLOT(onUnorderedDataCollectionPageAdded(ParsedPagePtr, int))));

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
	StorageType storageType = static_cast<StorageType>(type);

	const ISequencedStorage& storage = *m_sequencedDataCollection->storage(storageType);

	m_parsedPages[storageType].push_back(storage[row]);

	checkWaitCondition(storageType);
}

void ParsedPageReceiver::onParsedPageLinksToThisResourceChanged(LinksToThisResourceChanges changes)
{
	for (const LinksToThisResourceChanges::Change& change : changes.changes)
	{
		m_linksToThisResourceChanges[change.page.lock().get()].push_back(changes);
	}

	checkLinksToThisResourceConditions(nullptr);
}


void ParsedPageReceiver::onCrawlingProgress(CrawlingProgress state)
{
	if (!m_allPagesReceived && state.crawledLinkCount > 0 && state.pendingLinkCount == 0)
	{
		m_allPagesReceivedPromise.set_value(m_parsedPages[StorageType::CrawledUrlStorageType]);
		m_allPagesReceived = true;
	}
}

void ParsedPageReceiver::onUnorderedDataCollectionPageAdded(ParsedPagePtr page, int type)
{
	m_unorderedDataCollectionPages[type].push_back(page.get());
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

std::future<std::vector<const ParsedPage*>> ParsedPageReceiver::getParsedPages(int count, int storageType)
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