#pragma once

#include "requester.h"
#include "crawler_request.h"
#include "idownloader.h"

namespace CrawlerEngine
{

class RandomIntervalRangeTimer;
class IUniquenessChecker;
struct DownloadResponse;

class Downloader : public QObject, public IDownloader
{
	Q_OBJECT

public:
	Downloader();

	Q_INVOKABLE virtual void setPauseRange(int from, int to) override;
	Q_INVOKABLE virtual void setTimeout(int msecs) override;
	Q_INVOKABLE virtual void setMaxRedirects(int redirects) override;
	Q_INVOKABLE virtual void resetPauseRange() override;
	Q_INVOKABLE virtual void setUserAgent(const QByteArray& userAgent) override;
	Q_INVOKABLE virtual void setProxy(const QString& proxyHostName, int proxyPort, const QString& proxyUser, const QString& proxyPassword) override;
	Q_INVOKABLE virtual void resetProxy() override;
	Q_INVOKABLE virtual void handleRequest(RequesterSharedPtr requester) override;
	Q_INVOKABLE virtual void stopRequestHandling(RequesterSharedPtr requester) override;

	virtual QObject* qobject() override;

private slots:
	void urlDownloaded(QNetworkReply* reply);
	void metaDataChanged(QNetworkReply* reply);
	void queryError(QNetworkReply* reply, QNetworkReply::NetworkError code);
	void onTimerTicked();
	void onTimeoutTimerTicked();
	void proxyAuthenticationRequiredSlot(const QNetworkProxy&, QAuthenticator*) const;
	void onAboutDownloadProgress(qint64 bytesReceived, qint64 bytesTotal);
	bool isAutoDetectionBodyProcessing(QNetworkReply* reply) const;

private:
	void processReply(QNetworkReply* reply);
	bool isReplyProcessed(QNetworkReply* reply) const noexcept;
	void markReplyAsProcessed(QNetworkReply* reply) noexcept;
	void load(RequesterSharedPtr requester);
	int maxRedirectsToProcess() const noexcept;
	std::pair<int, QNetworkReply*> loadHelper(const CrawlerRequest& request, int parentRequestId = -1, bool useTimeout = false);

	std::shared_ptr<DownloadResponse> responseFor(int requestId);
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
	RandomIntervalRangeTimer* m_randomIntervalRangeTimer;
	QTimer* m_timeoutTimer;
	int m_timeout;
	int m_maxRedirects;
	std::deque<RequesterSharedPtr> m_requesterQueue;
	std::map<RequesterSharedPtr, QNetworkReply*> m_activeRequestersReplies;
};

}
