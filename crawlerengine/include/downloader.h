#pragma once

#include "requester.h"

namespace CrawlerEngine
{
	
class Downloader : public QObject
{
	Q_OBJECT

public:
	Downloader();

	Q_INVOKABLE void handleRequest(Common::RequesterSharedPtr requester);
	Q_INVOKABLE void stopRequestHandling(Common::RequesterSharedPtr requester);

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
	Common::RequesterWeakPtr m_requesterWeakPtr;
};

}