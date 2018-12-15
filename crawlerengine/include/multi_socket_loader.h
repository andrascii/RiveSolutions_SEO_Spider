#pragma once

#include "download_request.h"
#include "curl_http_headers_wrapper.h"
#include "response_headers.h"

namespace Common
{

enum class StatusCode;

}

namespace CrawlerEngine
{

class Url;
class HopsChain;

/*! Information associated with a specific HTTP request */
struct RequestDescriptor
{
	enum class InterruptionReason
	{
		ReasonContinueLoading,
		ReasonNonHtmlResponse
	};

	enum class Method
	{
		Get,
		Head,
		Post
	};

	RequestDescriptor(const std::function<void(int)>& onDestroyFunction)
		: onDestroyFunction(onDestroyFunction)
	{
	}

	~RequestDescriptor()
	{
		onDestroyFunction(id);
	}

	struct UploadData
	{
		//! buffer
		QByteArray buffer;
		//! this pointer used to point on data.constData();
		//! this pointer will move in process of reading
		const char* dataPointer;
		//! upload data length
		size_t dataLength;
	};

	//! CURL easy handle describing the request
	CURL* easy;
	//! URL for the request
	QByteArray url;
	//! stores body for GET method
	QByteArray body;
	//! used for POST method
	UploadData uploadData;
	//! server response headers
	ResponseHeaders responseHeaders;
	//! response status code received from the server
	Common::StatusCode statusCode;
	//! time point at the start of loading, used to calculate loading time
	std::chrono::time_point<std::chrono::high_resolution_clock> startLoadingPoint;
	//! used for mark the reason for interruption loading (method GET)
	InterruptionReason interruptionReason = InterruptionReason::ReasonContinueLoading;
	//! should we load body of non-HTML resource mark
	DownloadRequest::BodyProcessingCommand bodyProcessingCommand;
	//! Method to use for the request
	Method method;
	//! stores identifier assigned to the request
	int id = -1;
	//! this buffer stores CURL error message if it occurs
	char error[CURL_ERROR_SIZE];
	//! destroy function which is removes this descriptor by id from MultiSocketLoader::m_activeRequestDescriptors map
	std::function<void(int)> onDestroyFunction;
};

class MultiSocketLoader : public QObject
{
	Q_OBJECT

public:
	MultiSocketLoader(QObject* parent = nullptr);
	~MultiSocketLoader();

	int get(const Url& url, DownloadRequest::BodyProcessingCommand bodyProcessingCommand = DownloadRequest::BodyProcessingCommand::CommandAutoDetectionBodyLoading);
	int post(const Url& url, QByteArray uploadData);
	int head(const Url& url);

	void setProxySettings(const QString& proxyHostName, int proxyPort, const QString& proxyUser, const QString& proxyPassword);
	void resetProxySettings();

	void setTimeout(size_t timeoutMilliseconds);
	void resetTimeout();
	size_t timeout() const;

	void setUserAgent(const QByteArray& userAgent);

	int currentParallelConnections() const noexcept;

	void pauseConnection(int id) const;
	void unpauseConnection(int id);
	void resetConnection(int id);

signals:
	void loaded(int id,
		const QByteArray& url,
		const QByteArray& data,
		const ResponseHeaders& responseHeaders,
		Common::StatusCode statusCode,
		int timeElapsed);

	void downloadProgress(int id, double total, double received);
	void uploadProgress(int id, double total, double sent);
	void currentParallelTransferCountChanged(int count);

protected:
	virtual void timerEvent(QTimerEvent* event) override;

private slots:
	void doAction(curl_socket_t socketDescriptor, int curlAction);

private:
	void setTimer();
	void resetTimer();
	void applyProxySettingsIfNeeded(CURL* easyHandle) const;

	RequestDescriptor* createRequestDescriptor(const Url& url, size_t timeoutMilliseconds, curl_slist* headers);

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

private:
	static MultiSocketLoader* instance();
	static int socketFunctionCallback(CURL* easy, curl_socket_t s, int action, void* userPrivateData, void* socketPrivateData);
	static int timerFunctionCallback(CURLM* multi, long timeoutMs, void* userPrivateData);
	static size_t writeBodyCallback(void* buffer, size_t size, size_t nmemb, void* userData);
	static size_t writeResponseHeadersCallback(void* buffer, size_t size, size_t nmemb, void* userData);
	static size_t readUploadDataCallback(void* buffer, size_t size, size_t nmemb, void* userData);
	static int progressFunctionCallback(void* clientPointer, double downloadTotal, double downloadReceived, double uploadTotal, double uploadSent);

private:
	static MultiSocketLoader* s_instance;

private:
	QTimer* m_timer;
	SocketFunctionCallbackPrivateData m_socketPrivateData;
	ProxySettings m_proxySettings;
	CurlHttpHeadersWrapper m_headers;
	QMap<int, RequestDescriptor*> m_activeRequestDescriptors;
	size_t m_timeoutMilliseconds;
	int m_requestCounter;
	int m_timerId;
};

}
