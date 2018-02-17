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
	~ParsedPageReceiver();

	std::future<std::vector<const ParsedPage*>> getParsedPages(int count, StorageType storageType);
	std::future<std::vector<const ParsedPage*>> getAllCrawledPages();
	std::vector<const ParsedPage*> storageItems(StorageType storage) const;
	std::future<std::vector<LinksToThisResourceChanges>> getLinksToThisResourceChanges(const ParsedPage* page, int count);
	std::vector<const ParsedPage*> getLinksFromUnorderedDataCollection(StorageType type) const;

	void clearReceivedData();

private slots:
	void onParsedPageAdded(int row, StorageType type);
	void onParsedPageRemoved(int row, StorageType type);
	void onParsedPagesRemoved(int count, StorageType type);
	void onParsedPageLinksToThisResourceChanged(LinksToThisResourceChanges changes);
	void onCrawlingProgress(CrawlingProgress state);
	void onAboutClearData();
	void onUnorderedDataCollectionPageAdded(ParsedPagePtr page, StorageType type);

private:
	void checkWaitCondition(StorageType storageType);
	void checkLinksToThisResourceConditions(const ParsedPage* page);

private:
	QThread * m_receiverThread;

	std::map<StorageType, std::vector<const ParsedPage*>> m_parsedPages;
	std::map<StorageType, std::pair<int, std::promise<std::vector<const ParsedPage*>>>> m_waitConditions;

	std::map<const ParsedPage*, std::vector<LinksToThisResourceChanges>> m_linksToThisResourceChanges;
	std::map<const ParsedPage*, std::pair<int, std::promise<std::vector<LinksToThisResourceChanges>>>> m_linksToThisResourceConditions;

	const SequencedDataCollection* m_sequencedDataCollection;

	std::promise<std::vector<const ParsedPage*>> m_allPagesReceivedPromise;
	std::atomic_bool m_allPagesReceived;

	std::map<StorageType, std::vector<const ParsedPage*>> m_unorderedDataCollectionPages;
};

}