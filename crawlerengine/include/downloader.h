#pragma once

#include "requester.h"
#include "crawler_request.h"
#include "idownloader.h"
#include "random_interval_range_timer.h"
#include "iuniqueness_checker.h"

namespace CrawlerEngine
{
	
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

private:
	void processReply(QNetworkReply* reply);
	bool isReplyProcessed(QNetworkReply* reply) const noexcept;
	void markReplyAsProcessed(QNetworkReply* reply) const noexcept;
	void load(RequesterSharedPtr requester);
	void loadHelper(const CrawlerRequest& request);

private:
	QNetworkAccessManager* m_networkAccessor;
	QMap<CrawlerRequest, RequesterWeakPtr> m_requesters;
	QMap<CrawlerRequest, std::shared_ptr<DownloadResponse>> m_responses;
	QByteArray m_userAgent;
	Common::RandomIntervalRangeTimer* m_randomIntervalRangeTimer;
	std::queue<RequesterSharedPtr> m_requesterQueue;
	std::unique_ptr<IUniquenessChecker> m_uniquenessChecker;
};

}
