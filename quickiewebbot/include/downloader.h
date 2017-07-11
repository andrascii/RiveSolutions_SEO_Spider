#pragma once

namespace QuickieWebBot
{

class Downloader : public QObject
{
	Q_OBJECT

public:
	struct Response
	{
		QUrl url;
		QString responseBody;
		QString responseHeaderValuePairs;
	};
	
	Downloader();
	virtual ~Downloader();

	void start();
	void stop();
	void wait();

	void scheduleRequest(const QUrl& url) noexcept;
	void scheduleRequestList(const QList<QUrl>& urlList) noexcept;

private:
	Q_SLOT void urlDownloaded(QNetworkReply* reply);
	Q_SLOT void handleRequestQueue();
	Q_SLOT void stopHandlingRequestQueue();

private:
	QNetworkAccessManager* m_networkAccessManager;
	QThread m_thisThread;

	QMutex m_requestQueueMutex;
	QMutex m_repliesQueueMutex;
	QQueue<QUrl> m_requestQueue;
	QQueue<Response> m_repliesQueue;
};

}
