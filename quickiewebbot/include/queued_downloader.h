#pragma once

#include "abstract_threadable_object.h"

namespace QuickieWebBot
{

class QueuedDownloader : public AbstractThreadableObject
{
	Q_OBJECT

public:
	struct Reply
	{
		QUrl url;
		QByteArray responseBody;
		QByteArray responseHeaderValuePairs;
	};
	
	QueuedDownloader();
	virtual ~QueuedDownloader();

	void scheduleUrl(const QUrl& url) noexcept;

	bool extractReply(Reply& response) noexcept;

private:
	virtual void process() override;

	Q_SLOT void urlDownloaded(QNetworkReply* reply);

private:
	QNetworkAccessManager* m_networkAccessManager;

	std::mutex m_requestQueueMutex;
	std::mutex m_repliesQueueMutex;

	std::vector<QUrl> m_requestQueue;
	std::vector<Reply> m_repliesQueue;

	std::atomic<int> m_unprocessedRepliesCount;
	std::atomic<int> m_pendingReguestsCount;
};

}
