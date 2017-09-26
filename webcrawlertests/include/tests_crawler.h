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

private:
	Q_SLOT void onParsedPageAdded(int row, int storageType);

private:
	QThread* m_receiverThread;
	std::vector<WebCrawler::ParsedPagePtr> m_parsedPages;
	std::promise<std::vector<WebCrawler::ParsedPagePtr>> m_promise;

	WebCrawler::SequencedDataCollection* m_sequencedDataCollection;
	int m_count;
	int m_storageType;

};

class TestsCrawler : public WebCrawler::Crawler
{
	Q_OBJECT
public:
	TestsCrawler(unsigned int threadCount, const WebCrawler::CrawlerOptions& options);
	~TestsCrawler();

	std::vector<WebCrawler::ParsedPagePtr> waitForParsedPageReceived(int count, int seconds) const;
	Q_SLOT void startTestCrawler();

	void setCondition(std::function<void()> cond);

protected:
	virtual WebCrawler::IQueuedDownloader* createQueuedDownloader() const noexcept override;
	QThread* m_sequensedCollectionThread;
	WebCrawler::CrawlerOptions m_testCrawlerOptions;
	std::function<void()> m_cond;
};

}