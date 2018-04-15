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
	void proxyAuthenticationRequiredSlot(const QNetworkProxy&, QAuthenticator*) const;
	void onAboutDownloadProgress(qint64 bytesReceived, qint64 bytesTotal);
	bool isAutoDetectionBodyProcessing(QNetworkReply* reply) const;

private:
	void processReply(QNetworkReply* reply);
	bool isReplyProcessed(QNetworkReply* reply) const noexcept;
	void markReplyAsProcessed(QNetworkReply* reply) const noexcept;
	void load(RequesterSharedPtr requester);
	std::pair<int, QNetworkReply*> loadHelper(const CrawlerRequest& request, int parentRequestId = -1);

private:
	QNetworkAccessManager* m_networkAccessor;
	QMap<int, RequesterWeakPtr> m_requesters;
	QVector<int> m_activeRequests;
	QMap<int, std::shared_ptr<DownloadResponse>> m_responses;
	QByteArray m_userAgent;
	RandomIntervalRangeTimer* m_randomIntervalRangeTimer;
	std::deque<RequesterSharedPtr> m_requesterQueue;
	std::map<RequesterSharedPtr, QNetworkReply*> m_activeRequestersReplies;
};

}
