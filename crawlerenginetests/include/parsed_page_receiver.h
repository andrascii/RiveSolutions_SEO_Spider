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

    std::future<std::vector<const ParsedPage*>> getParsedPages(int count, int storageType);
    std::future<std::vector<const ParsedPage*>> getAllCrawledPages();
    std::vector<const ParsedPage*> storageItems(StorageType storage) const;

    std::future<std::vector<LinksToThisResourceChanges>> getLinksToThisResourceChanges(const ParsedPage* page, int count);

    std::vector<const ParsedPage*> getLinksFromUnorderedDataCollection(int type) const;

private:
    Q_SLOT void onParsedPageAdded(int row, int type);
    Q_SLOT void onParsedPageLinksToThisResourceChanged(LinksToThisResourceChanges changes);
    Q_SLOT void onCrawlingProgress(CrawlingProgress state);
	Q_SLOT void onAboutClearData();
    Q_SLOT void onUnorderedDataCollectionPageAdded(ParsedPagePtr page, int type);

    void checkWaitCondition(int storageType);
    void checkLinksToThisResourceConditions(const ParsedPage* page);

private:
    QThread * m_receiverThread;

    std::map<int, std::vector<const ParsedPage*>> m_parsedPages;
    
    std::map<int, std::pair<int, std::promise<std::vector<const ParsedPage*>>>> m_waitConditions;

    std::map<const ParsedPage*, std::vector<LinksToThisResourceChanges>> m_linksToThisResourceChanges;
    
    std::map<const ParsedPage*, std::pair<int, std::promise<std::vector<LinksToThisResourceChanges>>>> m_linksToThisResourceConditions;

    const SequencedDataCollection* m_sequencedDataCollection;

    std::promise<std::vector<const ParsedPage*>> m_allPagesReceivedPromise;
    
    std::atomic_bool m_allPagesReceived;

    std::map<int, std::vector<const ParsedPage*>> m_unorderedDataCollectionPages;
};

}