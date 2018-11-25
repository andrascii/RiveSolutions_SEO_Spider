#pragma once

#include "ihttp_client.h"

namespace CrawlerEngine
{

class RandomIntervalRangeTimer;
class CurlHttpRequest;

class HttpClient : public QObject, public IHttpClient
{
	Q_OBJECT

public:
	using OnOperationCompletedCallback = std::function<void(const HopsChain&)>;

	HttpClient(OnOperationCompletedCallback callback, QObject* parent);
	~HttpClient();

	virtual void get(const Url& url) override;
	virtual void post(const Url& url, const QByteArray& uploadData) override;
	virtual void setPauseRange(int from, int to) override;
	virtual void resetPauseRange() override;
	virtual void setTimeout(int msecs) override;
	virtual void setMaxRedirects(int redirects) override;
	virtual void setUserAgent(const QByteArray& userAgent) override;
	virtual void setProxy(const QString& proxyHostName, int proxyPort, const QString& proxyUser, const QString& proxyPassword) override;
	virtual void resetProxy() override;

	virtual QObject* qobject() const override;

signals:
	virtual void operationCompleted(const HopsChain& hopsChain) override;

private slots:
	void onTimerTicked();

private:
	void applyRequestSettings(CurlHttpRequest& request) const;
	void getInternal(const Url& url);
	void postInternal(const Url& url, const QByteArray& uploadData);

private:
	enum class Method
	{
		Get,
		Post
	};

	struct PendingNetworkRequest
	{
		Url url;
		Method method;
		QByteArray uploadData;
	};

	struct ProxySettings
	{
		QString proxyHostName;
		int proxyPort;
		QString proxyUser;
		QString proxyPassword;
	};

	CURL* m_connection;
	RandomIntervalRangeTimer* m_randomIntervalRangeTimer;
	std::deque<PendingNetworkRequest> m_pendingRequestsQueue;
	int m_timeout;
	int m_maxRedirectCount;
	QByteArray m_userAgent;
	ProxySettings m_proxySettings;
	OnOperationCompletedCallback m_operationCompletedCallback;
};

}
