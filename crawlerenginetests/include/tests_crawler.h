#pragma once

#include "crawler.h"
#include "sequenced_data_collection.h"
#include "parsed_page_receiver.h"

namespace CrawlerEngine
{

class IHostInfoProvider;

}

namespace CrawlerEngineTests
{

using namespace CrawlerEngine;

class TestsDownloader;

class TestsCrawler : public Crawler
{
	Q_OBJECT

public:
	TestsCrawler(unsigned int threadCount, QObject* parent = nullptr);

	~TestsCrawler();

	Q_INVOKABLE void initialize();

	void waitForCrawlingDone();
	std::vector<const ParsedPage*> waitForParsedPageReceived(StorageType storage, int count, int seconds, const char* timeoutMessage) const;
	std::vector<const ParsedPage*> waitForAllCrawledPageReceived(int seconds, const char* timeoutMessage = "Waiting for all pages received") const;
	void waitForSerializationDone(int seconds, const char* timeoutMessage = "Waiting for serialization") const;
	void waitForDeserializationDone(int seconds, const char* timeoutMessage = "Waiting for deserialization") const;
	void waitForRefreshPageDone(int seconds, const char* timeoutMessage = "Waiting for refresh page") const;
	void waitForClearingDataDone(int seconds, const char* timeoutMessage = "Waiting for refresh page") const;
	std::vector<const ParsedPage*> storageItems(StorageType storage) const;
	std::vector<LinksToThisResourceChanges> waitForLinksToThisResourceChangesReceived(const ParsedPage* page, int count, int seconds) const;
	std::vector<const ParsedPage*> getLinksFromUnorderedDataCollection(StorageType type) const;

	void clearReceivedData();

	TestsDownloader* testDownloader() const;
	void checkSequencedDataCollectionConsistency();
	const UnorderedDataCollection* unorderedDataCollection() const;
	CrawlerOptions crawlerOptions() const;

	void saveToFileSafe(const QString& fileName);
	void loadFromFIleSafe(const QString& fileName);

protected:
	virtual IHostInfoProvider* createHostInfoProvider() const override;
	virtual IWebScreenShot* createWebScreenShot() override;
	virtual IDownloader* createDownloader() const override;
	virtual void initSequencedDataCollection() override;

private:
	QThread* m_sequencedDataCollectionThread;
	QThread* m_receiverThread;
	std::unique_ptr<ParsedPageReceiver> m_receiver;

	mutable TestsDownloader* m_downloader;
};

}