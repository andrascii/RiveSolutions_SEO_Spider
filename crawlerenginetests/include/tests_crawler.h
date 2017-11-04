#pragma once
#include "crawler.h"
#include "sequenced_data_collection.h"

namespace CrawlerEngineTests
{

class ParsedPageReceiver: public QObject
{
	Q_OBJECT
public:
	ParsedPageReceiver(CrawlerEngine::SequencedDataCollection* sequencedDataCollection);
	~ParsedPageReceiver();

	std::future<std::vector<CrawlerEngine::ParsedPagePtr>> getParsedPages(int count, int storageType);
	std::vector<CrawlerEngine::ParsedPagePtr> storageItems(CrawlerEngine::StorageType storage) const;

	std::future<std::vector<CrawlerEngine::LinksToThisResourceChanges>> getLinksToThisResourceChanges(CrawlerEngine::ParsedPagePtr page, int count);

private:
	Q_SLOT void onParsedPageAdded(int row, int storageType);
	Q_SLOT void onParsedPageLinksToThisResourceChanged(CrawlerEngine::LinksToThisResourceChanges changes);

	void checkWaitCondition(int storageType);
	void checkLinksToThisResourceConditions(CrawlerEngine::ParsedPagePtr page);

private:
	QThread* m_receiverThread;
	std::map<int, std::vector<CrawlerEngine::ParsedPagePtr>> m_parsedPages;
	std::map<int, std::pair<int, std::promise<std::vector<CrawlerEngine::ParsedPagePtr>>>> m_waitConditions;
	
	std::map<CrawlerEngine::ParsedPagePtr, std::vector<CrawlerEngine::LinksToThisResourceChanges>> m_linksToThisResourceChanges;
	std::map<CrawlerEngine::ParsedPagePtr, std::pair<int, std::promise<std::vector<CrawlerEngine::LinksToThisResourceChanges>>>> m_linksToThisResourceConditions;

	CrawlerEngine::SequencedDataCollection* m_sequencedDataCollection;
};

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

	std::vector<CrawlerEngine::ParsedPagePtr> waitForParsedPageReceived(
		CrawlerEngine::StorageType storage, int count, int seconds, const char* timeoutMessage) const;

	std::vector<CrawlerEngine::ParsedPagePtr> storageItems(CrawlerEngine::StorageType storage) const;

	std::vector<CrawlerEngine::LinksToThisResourceChanges> waitForLinksToThisResourceChangesReceived(
		CrawlerEngine::ParsedPagePtr page, int count, int seconds) const;

	Q_SLOT void startTestCrawler();

protected:
	virtual CrawlerEngine::IDownloader* createDownloader() const override;
	virtual CrawlerEngine::IRobotsTxtLoader* createRobotsTxtLoader() const override;

private:

	CrawlerEngine::CrawlerOptions m_testCrawlerOptions;

	QThread* m_sequencedDataCollectionThread;

	std::unique_ptr<ParsedPageReceiver> m_receiver;
};

}