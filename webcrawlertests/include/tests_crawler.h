#pragma once
#include "crawler.h"
#include "sequenced_data_collection.h"

namespace WebCrawlerTests
{

class ParsedPageReceiver: public QObject
{
	Q_OBJECT
public:
	ParsedPageReceiver(WebCrawler::SequencedDataCollection* sequencedDataCollection);
	~ParsedPageReceiver();

	std::future<std::vector<WebCrawler::ParsedPagePtr>> getParsedPages(int count, int storageType);
	std::vector<WebCrawler::ParsedPagePtr> storageItems(WebCrawler::StorageType storage) const;

	std::future<std::vector<WebCrawler::LinksToThisResourceChanges>> getLinksToThisResourceChanges(WebCrawler::ParsedPagePtr page, int count);

private:
	Q_SLOT void onParsedPageAdded(int row, int storageType);
	Q_SLOT void onParsedPageLinksToThisResourceChanged(WebCrawler::LinksToThisResourceChanges changes);

	void checkWaitCondition(int storageType);
	void checkLinksToThisResourceConditions(WebCrawler::ParsedPagePtr page);

private:
	QThread* m_receiverThread;
	std::map<int, std::vector<WebCrawler::ParsedPagePtr>> m_parsedPages;
	std::map<int, std::pair<int, std::promise<std::vector<WebCrawler::ParsedPagePtr>>>> m_waitConditions;
	
	std::map<WebCrawler::ParsedPagePtr, std::vector<WebCrawler::LinksToThisResourceChanges>> m_linksToThisResourceChanges;
	std::map<WebCrawler::ParsedPagePtr, std::pair<int, std::promise<std::vector<WebCrawler::LinksToThisResourceChanges>>>> m_linksToThisResourceConditions;

	WebCrawler::SequencedDataCollection* m_sequencedDataCollection;

};

class TestsCrawler : public WebCrawler::Crawler
{
	Q_OBJECT
public:
	TestsCrawler(unsigned int threadCount, const WebCrawler::CrawlerOptions& options);
	~TestsCrawler();

	std::vector<WebCrawler::ParsedPagePtr> waitForParsedPageReceived(WebCrawler::StorageType storage, int count, int seconds, const char* timeoutMessage) const;
	std::vector<WebCrawler::ParsedPagePtr> storageItems(WebCrawler::StorageType storage) const;

	std::vector<WebCrawler::LinksToThisResourceChanges> waitForLinksToThisResourceChangesReceived(WebCrawler::ParsedPagePtr page, int count, int seconds) const;

	Q_SLOT void startTestCrawler();

	void setCondition(std::function<void()> cond);

protected:
	virtual WebCrawler::IQueuedDownloader* createQueuedDownloader() const noexcept override;
	virtual WebCrawler::IRobotsTxtLoader* createRobotsTxtLoader() const noexcept override;
	QThread* m_sequensedCollectionThread;
	WebCrawler::CrawlerOptions m_testCrawlerOptions;
	std::function<void()> m_cond;

	std::unique_ptr<ParsedPageReceiver> m_receiver;
};

}