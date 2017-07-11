#pragma once

namespace QuickieWebBot
{

class QueuedDownloader : public QObject
{
	Q_OBJECT

public:
	enum ResponseExtractType
	{
		SuspendExtractType,
		AsyncExtractType
	};

	struct Response
	{
		QUrl url;
		QString responseBody;
		QString responseHeaderValuePairs;
	};
	
	QueuedDownloader();
	virtual ~QueuedDownloader();

	bool isRunning() const noexcept;

	void start();
	void stop();

	void scheduleUrl(const QUrl& url) noexcept;
	void scheduleUrlList(const QList<QUrl>& urlList) noexcept;

	bool extractResponse(Response& response, ResponseExtractType type) noexcept;

protected:
	virtual void timerEvent(QTimerEvent* event) override;

private:
	void wait();

	Q_SLOT void urlDownloaded(QNetworkReply* reply);
	Q_SLOT void processUrlQueue();
	Q_SLOT void stopProcessingUrlQueue();

	Q_INVOKABLE void startTimer(int interval, Qt::TimerType timerType = Qt::CoarseTimer);
	Q_INVOKABLE void killTimer(int timerId);

private:
	QNetworkAccessManager* m_networkAccessManager;
	QThread m_thisThread;

	std::mutex m_requestQueueMutex;
	std::mutex m_repliesQueueMutex;
	std::condition_variable m_repliesWaitCondition;
	QQueue<QUrl> m_requestQueue;
	QQueue<Response> m_repliesQueue;

	int m_timerId;
	bool m_isRunning;
};

}
