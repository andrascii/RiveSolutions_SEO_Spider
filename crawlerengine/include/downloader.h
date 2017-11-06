#pragma once

#include "requester.h"
#include "crawler_request.h"
#include "idownloader.h"

namespace CrawlerEngine
{
	
class Downloader : public QObject, public IDownloader
{
	Q_OBJECT

public:
	Downloader();

	virtual void setUserAgent(const QByteArray& userAgent) override;

	Q_INVOKABLE virtual void handleRequest(RequesterSharedPtr requester) override;

	Q_INVOKABLE virtual void stopRequestHandling(RequesterSharedPtr requester) override;

	virtual QObject* qobject() override;

private slots:
	void urlDownloaded(QNetworkReply* reply);

	void metaDataChanged(QNetworkReply* reply);

	void queryError(QNetworkReply* reply, QNetworkReply::NetworkError code);

private:
	void processReply(QNetworkReply* reply);

	bool isReplyProcessed(QNetworkReply* reply) const noexcept;

	void markReplyAsProcessed(QNetworkReply* reply) const noexcept;

private:
	QNetworkAccessManager* m_networkAccessor;

	std::map<CrawlerRequest, RequesterWeakPtr> m_requesters;

	QByteArray m_userAgent;
};

}
