#pragma once

#include "crawler.h"
#include "sequenced_data_collection.h"
#include "parsed_page_receiver.h"

namespace CrawlerEngineTests
{

class TestsDownloader;

class TestsCrawler : public CrawlerEngine::Crawler
{
	Q_OBJECT

public:
	TestsCrawler(
		unsigned int threadCount, 
		const CrawlerEngine::CrawlerOptions& options, 
		QObject* parent = nullptr
	);

	~TestsCrawler();

	std::vector<const CrawlerEngine::ParsedPage*> waitForParsedPageReceived(
		CrawlerEngine::StorageType storage, int count, int seconds, const char* timeoutMessage) const;

	std::vector<const CrawlerEngine::ParsedPage*> storageItems(CrawlerEngine::StorageType storage) const;

	std::vector<CrawlerEngine::LinksToThisResourceChanges> waitForLinksToThisResourceChangesReceived(
		const CrawlerEngine::ParsedPage* page, int count, int seconds) const;

	Q_SLOT void startTestCrawler();

	TestsDownloader* testDownloader() const;

	void checkSequencedDataCollectionConsistency();

protected:
	virtual CrawlerEngine::IDownloader* createDownloader() const override;
	virtual CrawlerEngine::IRobotsTxtLoader* createRobotsTxtLoader() const override;
	void createSequencedDataCollection(QThread* targetThread) const;

private:
	static std::mutex s_mutex;

	CrawlerEngine::CrawlerOptions m_testCrawlerOptions;
	QThread* m_sequencedDataCollectionThread;
	std::unique_ptr<ParsedPageReceiver> m_receiver;

	mutable TestsDownloader* m_downloader;
};

}