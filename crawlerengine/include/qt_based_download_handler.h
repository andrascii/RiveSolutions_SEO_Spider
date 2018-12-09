#pragma once

#include "requester.h"
#include "crawler_request.h"
#include "abstract_download_handler.h"

namespace CrawlerEngine
{

class RandomIntervalRangeTimer;
struct DownloadResponse;

class QtBasedDownloadHandler : public AbstractDownloadHandler
{
	Q_OBJECT

public:
	QtBasedDownloadHandler();

	Q_INVOKABLE virtual void setTimeout(int msecs) override;
	Q_INVOKABLE virtual void setUserAgent(const QByteArray& userAgent) override;
	Q_INVOKABLE virtual void setProxy(const QString& proxyHostName, int proxyPort, const QString& proxyUser, const QString& proxyPassword) override;
	Q_INVOKABLE virtual void resetProxy() override;
	Q_INVOKABLE virtual void stopRequestHandling(RequesterSharedPtr requester) override;

	virtual QObject* qobject() override;

private slots:
	void urlDownloaded(QNetworkReply* reply);
	void metaDataChanged(QNetworkReply* reply);
	void queryError(QNetworkReply* reply, QNetworkReply::NetworkError code);
	void onTimeoutTimerTicked();
	void onAboutDownloadProgress(qint64 bytesReceived, qint64 bytesTotal);
	bool isAutoDetectionBodyProcessing(QNetworkReply* reply) const;

private:
	virtual void load(RequesterSharedPtr requester) override;
	virtual std::shared_ptr<DownloadResponse> responseFor(int requestId) override;
	virtual void pauseRequesters(const QVector<const void*>& requesterToBePaused) override;
	virtual void unpauseRequesters(const QVector<const void*>& requesterToBeUnpaused) override;

private:
	void processReply(QNetworkReply* reply);
	bool isReplyProcessed(QNetworkReply* reply) const noexcept;
	void markReplyAsProcessed(QNetworkReply* reply) noexcept;
	std::pair<int, QNetworkReply*> loadHelper(const CrawlerRequest& request, int parentRequestId = -1, bool useTimeout = false);

	QByteArray readBody(QNetworkReply* reply) const;
	Common::StatusCode replyStatusCode(QNetworkReply* reply) const;
	Url redirectUrl(QNetworkReply* reply) const;

private:
	QNetworkAccessManager* m_networkAccessor;
	QMap<int, RequesterWeakPtr> m_requesters;
	QMap<QNetworkReply*, QPair<QPointer<QNetworkReply>, qint64>> m_activeReplies;
	QVector<int> m_activeRequests;
	QMap<int, std::shared_ptr<DownloadResponse>> m_responses;
	QByteArray m_userAgent;
	QTimer* m_timeoutTimer;
	int m_timeout;
	std::map<RequesterSharedPtr, QNetworkReply*> m_activeRequestersReplies;
};

}
