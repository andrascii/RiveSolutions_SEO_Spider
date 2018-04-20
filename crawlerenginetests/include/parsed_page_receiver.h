#pragma once

namespace CrawlerEngineTests
{

using namespace CrawlerEngine;

class TestsCrawler;

class ParsedPageReceiver : public QObject
{
	Q_OBJECT

public:
	ParsedPageReceiver(const TestsCrawler* crawler, const SequencedDataCollection* sequencedDataCollection);

	std::future<std::vector<const ParsedPage*>> getParsedPages(int count, StorageType storageType);
	std::future<std::vector<const ParsedPage*>> getAllCrawledPages();
	std::vector<const ParsedPage*> storageItems(StorageType storage) const;
	std::future<std::vector<LinksToThisResourceChanges>> getLinksToThisResourceChanges(const ParsedPage* page, int count);
	std::vector<const ParsedPage*> getLinksFromUnorderedDataCollection(StorageType type) const;
	std::future<void> getWaitForClearingData();
	std::future<void> getWaitForSerializationDone();
	std::future<void> getWaitForDeserializationDone();

	void wait();

	void clearReceivedData();

private slots:
	void onParsedPageAdded(int row, StorageType type);
	void onParsedPageRemoved(int row, StorageType type);
	void onParsedPagesRemoved(int count, StorageType type);
	void onParsedPageLinksToThisResourceChanged(LinksToThisResourceChanges changes);
	void onCrawlerStarted();
	void onCrawlerFinished();
	void onAboutClearData();
	void onDataCleared();
	void onSerializationDone();
	void onDeserializationDone();
	void onUnorderedDataCollectionPageAdded(ParsedPagePtr page, StorageType type);
	void onUnorderedDataCollectionPageAdded(WorkerResult result, StorageType type);
	void onUnorderedDataCollectionPageRemoved(ParsedPagePtr page, StorageType type);

private:
	void checkWaitCondition(StorageType storageType);
	void checkLinksToThisResourceConditions(const ParsedPage* page);
	void checkAllPagesReceived();

private:
	QThread* m_receiverThread;

	std::map<StorageType, std::vector<const ParsedPage*>> m_parsedPages;
	std::map<StorageType, std::pair<int, std::promise<std::vector<const ParsedPage*>>>> m_waitConditions;

	std::map<const ParsedPage*, std::vector<LinksToThisResourceChanges>> m_linksToThisResourceChanges;
	std::map<const ParsedPage*, std::pair<int, std::promise<std::vector<LinksToThisResourceChanges>>>> m_linksToThisResourceConditions;

	const SequencedDataCollection* m_sequencedDataCollection;

	std::promise<std::vector<const ParsedPage*>> m_allPagesReceivedPromise;
	std::atomic_bool m_allPagesReceived;

	mutable std::mutex m_ucMutex;
	std::map<StorageType, std::vector<const ParsedPage*>> m_unorderedDataCollectionPages;

	std::promise<void> m_waitForClearingData;
	std::promise<void> m_waitForSerializationDone;
	std::promise<void> m_waitForDeserializationDone;
	bool m_finished;
};

}