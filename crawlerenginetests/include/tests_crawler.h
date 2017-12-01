#pragma once

#include "crawler.h"
#include "sequenced_data_collection.h"
#include "parsed_page_receiver.h"

namespace CrawlerEngineTests
{

using namespace CrawlerEngine;

class TestsDownloader;

class TestsCrawler : public Crawler
{
    Q_OBJECT

public:
    TestsCrawler(unsigned int threadCount, const CrawlerOptions& options, QObject* parent = nullptr);

    ~TestsCrawler();

    std::vector<const ParsedPage*> waitForParsedPageReceived(StorageType storage, int count, int seconds, const char* timeoutMessage) const;

    std::vector<const ParsedPage*> waitForAllCrawledPageReceived(int seconds, const char* timeoutMessage = "Waiting for all pages received") const;

    std::vector<const ParsedPage*> storageItems(StorageType storage) const;

    std::vector<LinksToThisResourceChanges> waitForLinksToThisResourceChangesReceived(const ParsedPage* page, int count, int seconds) const;

    std::vector<const ParsedPage*> getLinksFromUnorderedDataCollection(int type) const;

    Q_SLOT void startTestCrawler();

    TestsDownloader* testDownloader() const;

    void checkSequencedDataCollectionConsistency();

    const UnorderedDataCollection* unorderedDataCollection() const;

protected:
    virtual IDownloader* createDownloader() const override;
    virtual IRobotsTxtLoader* createRobotsTxtLoader() const override;
    virtual void createSequencedDataCollection(QThread* targetThread) const override;

private:
    CrawlerOptions m_testCrawlerOptions;

    QThread* m_sequencedDataCollectionThread;

    std::unique_ptr<ParsedPageReceiver> m_receiver;

    mutable TestsDownloader* m_downloader;
};

}