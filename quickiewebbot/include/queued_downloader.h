#pragma once

#include "abstract_threadable_object.h"

namespace QuickieWebBot
{

class QueuedDownloader : public AbstractThreadableObject
{
	Q_OBJECT

public:
	enum ReplyExtractPolicy
	{
		SuspendExtractPolicy,
		AsyncExtractPolicy
	};

	struct Reply
	{
		QUrl url;
		QString responseBody;
		QString responseHeaderValuePairs;
	};
	
	QueuedDownloader();
	virtual ~QueuedDownloader();

	void stop();

	void scheduleUrl(const QUrl& url) noexcept;
	void scheduleUrlList(const QList<QUrl>& urlList) noexcept;

	bool extractReply(Reply& response, ReplyExtractPolicy type) noexcept;

private:
	virtual void process() override;

	Q_SLOT void urlDownloaded(QNetworkReply* reply);

private:
	QNetworkAccessManager* m_networkAccessManager;

	std::mutex m_requestQueueMutex;
	std::mutex m_repliesQueueMutex;
	std::condition_variable m_repliesWaitCondition;

	QQueue<QUrl> m_requestQueue;
	QQueue<Reply> m_repliesQueue;
};

}
