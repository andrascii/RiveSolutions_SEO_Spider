#pragma once

#include "iqueued_dowloader.h"

namespace WebCrawler
{

class QueuedDownloader : public AbstractThreadableObject, public IQueuedDownloader
{
	Q_OBJECT

public:
	QueuedDownloader();
	virtual ~QueuedDownloader();

	virtual void scheduleUrl(const CrawlerRequest& url) noexcept override;

	virtual bool extractReply(Reply& response) noexcept override;

	virtual void start() noexcept override;
	virtual void stop() noexcept override;

private:
	virtual void process() override;

	Q_SLOT void urlDownloaded(QNetworkReply* reply);
	Q_SLOT void metaDataChanged(QNetworkReply* reply);
	Q_SLOT void queryError(QNetworkReply* reply, QNetworkReply::NetworkError code);

	void processReply(QNetworkReply* reply);

	void markReplyAsProcessed(QNetworkReply* reply) const noexcept;
	bool isReplyProcessed(QNetworkReply* reply) const noexcept;

private:
	QNetworkAccessManager* m_networkAccessManager;

	std::mutex m_requestQueueMutex;
	std::mutex m_repliesQueueMutex;

	std::vector<CrawlerRequest> m_requestQueue;
	std::vector<Reply> m_repliesQueue;

	std::atomic<int> m_unprocessedRepliesCount;
	std::atomic<int> m_pendingRequestsCount;
};

}
