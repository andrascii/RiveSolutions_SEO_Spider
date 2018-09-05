#pragma once

#include "icustom_data_feed.h"
#include "requester_wrapper.h"

namespace CrawlerEngine
{
class Requester;
struct DownloadResponse;

class YandexMetricaAuthenticator: public QObject, public ICustomDataFeedAuthenticator
{
	Q_OBJECT

public:
	YandexMetricaAuthenticator(QObject* parent);
	virtual bool authenticated() const override;
	virtual void authenticate() override;
	Q_SIGNAL virtual void authenticationDone(const QVariantMap& data) override;
	QByteArray token() const;

private:
	Q_SLOT void onCustomUriReceived(const QByteArray& url);

private:
	QByteArray m_token;
	QDateTime m_tokenExpireTime;
};

class YandexMetricaDataFeed: public QObject, public ICustomDataFeed
{
	Q_OBJECT

public:
	YandexMetricaDataFeed();
	virtual DataFeedId dataFeedId() const override;
	virtual QString name() const override;
	virtual QStringList columns() const override;
	virtual void requestData(ParsedPageWeakPtr page) override;
	virtual bool connected() const override;
	virtual void connect() override;
	virtual void setConnectionData(const QVariantMap& data) override;
	virtual ICustomDataFeedAuthenticator* authenticator() const override;
	virtual QVariantMap connectionData() const override;
	virtual void setParams(const QVariantMap& params) override;

	// signals
	Q_SIGNAL virtual void dataReady(ICustomDataFeedRow* row) override;

private:
	void onLoadingDone(Requester* requester, const DownloadResponse& response);

private:
	YandexMetricaAuthenticator* m_authenticator;
	RequesterWrapper m_downloadRequester;
};

}
