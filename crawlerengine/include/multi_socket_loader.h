#pragma once

#include "download_request.h"
#include "curl_http_headers_wrapper.h"

namespace Common
{

enum class StatusCode;

}

namespace CrawlerEngine
{

class Url;
class HopsChain;
class ResponseHeaders;

class MultiSocketLoader : public QObject
{
	Q_OBJECT

public:
	MultiSocketLoader(QObject* parent = nullptr);
	~MultiSocketLoader();

	int get(const Url& url, DownloadRequest::BodyProcessingCommand bodyProcessingCommand = DownloadRequest::BodyProcessingCommand::CommandAutoDetectionBodyLoading);
	int post(const Url& url);
	int head(const Url& url);

	void setProxySettings(const QString& proxyHostName, int proxyPort, const QString& proxyUser, const QString& proxyPassword);
	void resetProxySettings();

	void setTimeout(size_t timeoutMilliseconds);
	void resetTimeout();
	size_t timeout() const;

	void setUserAgent(const QByteArray& userAgent);

	static MultiSocketLoader* instance();

signals:
	void loaded(int id,
		const QByteArray& url,
		const QByteArray& data,
		const ResponseHeaders& responseHeaders,
		Common::StatusCode statusCode,
		int timeElapsed);

protected:
	virtual void timerEvent(QTimerEvent* event) override;

private slots:
	void doAction(curl_socket_t socketDescriptor, int curlAction);

private:
	void setTimer();
	void resetTimer();
	void applyProxySettingsIfNeeded(CURL* easyHandle) const;

private:
	struct SocketFunctionCallbackPrivateData
	{
		MultiSocketLoader* multiSocketLoader;
		CURLM* multiHandle;
		int stillRunning;
	};

	struct ProxySettings
	{
		QString proxyHostName;
		int proxyPort;
		QString proxyUser;
		QString proxyPassword;
	};

	static int socketFunctionCallback(CURL* easy, curl_socket_t s, int action, void* userPrivateData, void* socketPrivateData);
	static int timerFunctionCallback(CURLM* multi, long timeoutMs, void* userPrivateData);

private:
	QTimer* m_timer;
	SocketFunctionCallbackPrivateData m_socketPrivateData;
	size_t m_timeoutMilliseconds;
	ProxySettings m_proxySettings;
	int m_requestCounter;
	CurlHttpHeadersWrapper m_headers;
	int m_timerId;
};

}
