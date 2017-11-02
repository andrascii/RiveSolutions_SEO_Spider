#pragma once
#include "iqueued_dowloader.h"

namespace CrawlerEngineTests
{
	
class TestsQueudedDownoader : public CrawlerEngine::IQueuedDownloader
{
	virtual void setUserAgent(const QByteArray& userAgent) override;

	virtual void scheduleUrl(const CrawlerEngine::CrawlerRequest& url) noexcept override;
	virtual bool extractReply(Reply& response) noexcept override;

	virtual void start() noexcept override;
	virtual void stop() noexcept override;

	virtual size_t unprocessedRequestCount() const noexcept override;

private:
	QDir testsDataDir() const;
	std::pair<QString, QString> mapUrlToTestDataFiles(const CrawlerEngine::CrawlerRequest& url) const;

private:
	std::vector<Reply> m_repliesQueue;
	std::mutex m_repliesQueueMutex;
	mutable QString m_testDataPath;
};

}