#pragma once

#include "abstract_threadable_object.h"
#include "crawler_request.h"

namespace WebCrawler
{

class QueuedDownloader : public AbstractThreadableObject
{
	Q_OBJECT

public:
	struct Reply
	{
		int statusCode;
		QUrl url;
		QUrl redirectUrl;
		QByteArray responseBody;
		QByteArray responseHeaderValuePairs;
		QMap<QByteArray, QByteArray> responseHeaders;
	};
	
	QueuedDownloader();
	virtual ~QueuedDownloader();

	void scheduleUrl(const CrawlerRequest& url) noexcept;

	bool extractReply(Reply& response) noexcept;

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
