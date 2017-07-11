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

protected:
	virtual void timerEvent(QTimerEvent* event) override;

private:
	Q_SLOT void urlDownloaded(QNetworkReply* reply);
	Q_SLOT void handleRequestQueue();
	Q_SLOT void stopHandlingRequestQueue();

	Q_INVOKABLE void startTimer(int interval, Qt::TimerType timerType = Qt::CoarseTimer);
	Q_INVOKABLE void killTimer(int timerId);

private:
	QNetworkAccessManager* m_networkAccessManager;
	QThread m_thisThread;

	QMutex m_requestQueueMutex;
	QMutex m_repliesQueueMutex;
	QQueue<QUrl> m_requestQueue;
	QQueue<Response> m_repliesQueue;

	int m_timerId;
};

}
