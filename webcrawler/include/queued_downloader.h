#pragma once

#include "iqueued_dowloader.h"

namespace WebCrawler
{

// all methods must be a thread-safe

class QueuedDownloader : public AbstractThreadableObject, public IQueuedDownloader
{
	Q_OBJECT

public:
	QueuedDownloader();
	virtual ~QueuedDownloader();

	virtual void setUserAgent(const QByteArray& userAgent) override;

	virtual void scheduleUrl(const CrawlerRequest& url) noexcept override;
	virtual bool extractReply(Reply& response) noexcept override;

	virtual void start() noexcept override;
	virtual void stop() noexcept override;

	virtual size_t unprocessedRequestCount() const noexcept override;

private slots:
	void urlDownloaded(QNetworkReply* reply);
	void metaDataChanged(QNetworkReply* reply);
	void queryError(QNetworkReply* reply, QNetworkReply::NetworkError code);

private:
	virtual void process() override;
	void processReply(QNetworkReply* reply);
	void markReplyAsProcessed(QNetworkReply* reply) const noexcept;
	bool isReplyProcessed(QNetworkReply* reply) const noexcept;

private:
	QNetworkAccessManager* m_networkAccessManager;

	mutable std::mutex m_requestQueueMutex;
	mutable std::mutex m_repliesQueueMutex;

	std::vector<CrawlerRequest> m_requestQueue;
	std::vector<Reply> m_repliesQueue;

	std::atomic<int> m_unprocessedRepliesCount;
	std::atomic<int> m_pendingRequestsCount;

	QByteArray m_userAgent;
};

}
