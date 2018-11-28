#pragma once

namespace Common {
	enum class StatusCode;
}

namespace CrawlerEngine
{

// IMPLEMENT:
// 1. Using proxy							#DONE
// 2. Loading elapsed time tracking			#DONE
// 3. POST requests
// 4. HEAD requests
// 5. Timeout per request					#DONE
// 6. Loading body of non html resources

class Url;
class HopsChain;
class ResponseHeaders;

class MultiSocketLoader : public QObject
{
	Q_OBJECT

public:
	MultiSocketLoader(QObject* parent = nullptr);
	~MultiSocketLoader();

	int get(const Url& url) const;
	int post(const Url& url) const;
	int head(const Url& url) const;

	void setProxySettings(const QString& proxyHostName, int proxyPort, const QString& proxyUser, const QString& proxyPassword);
	void resetProxySettings();

	void setTimeout(size_t timeoutMilliseconds);
	void resetTimeout();
	size_t timeout() const;

	static MultiSocketLoader* instance();

signals:
	void loaded(int id,
		const QByteArray& url,
		const QByteArray& data,
		const ResponseHeaders& responseHeaders,
		Common::StatusCode statusCode,
		int timeElapsed);

private slots:
	void onTimeout();
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
};

}
