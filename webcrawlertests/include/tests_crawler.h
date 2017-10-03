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

private:
	Q_SLOT void onParsedPageAdded(int row, int storageType);
	void checkWaitCondition(int storageType);

private:
	QThread* m_receiverThread;
	std::map<int, std::vector<WebCrawler::ParsedPagePtr>> m_parsedPages;
	std::map<int, std::pair<int, std::promise<std::vector<WebCrawler::ParsedPagePtr>>>> m_waitConditions;

	WebCrawler::SequencedDataCollection* m_sequencedDataCollection;

};

class TestsCrawler : public WebCrawler::Crawler
{
	Q_OBJECT
public:
	TestsCrawler(unsigned int threadCount, const WebCrawler::CrawlerOptions& options);
	~TestsCrawler();

	std::vector<WebCrawler::ParsedPagePtr> waitForParsedPageReceived(WebCrawler::StorageType storage, int count, int seconds) const;
	std::vector<WebCrawler::ParsedPagePtr> storageItems(WebCrawler::StorageType storage) const;
	Q_SLOT void startTestCrawler();

	void setCondition(std::function<void()> cond);

protected:
	virtual WebCrawler::IQueuedDownloader* createQueuedDownloader() const noexcept override;
	QThread* m_sequensedCollectionThread;
	WebCrawler::CrawlerOptions m_testCrawlerOptions;
	std::function<void()> m_cond;

	std::unique_ptr<ParsedPageReceiver> m_receiver;
};

}