#include "multi_socket_loader.h"
#include "page_parser_helpers.h"
#include "status_code_description.h"
#include "hops_chain.h"

namespace
{

using namespace CrawlerEngine;

constexpr size_t c_second = 1000;

/* Information associated with a specific HTTP request */
struct RequestDescriptor
{
	enum class InterruptionReason
	{
		ReasonContinueLoading,
		ReasonNonHtmlResponse
	};

	CURL* easy;
	QByteArray url;
	QByteArray data;
	ResponseHeaders responseHeaders;
	Common::StatusCode statusCode;
	std::chrono::time_point<std::chrono::high_resolution_clock> startLoadingPoint;
	InterruptionReason interruptionReason = InterruptionReason::ReasonContinueLoading;
	int id = 0;
	char error[CURL_ERROR_SIZE];
};

struct SocketInfo
{
	curl_socket_t sockfd;
	CURL* easy;
	int action;
};

void verifyCurlReturnCode(const char* where, CURLMcode code);
long getHttpCode(CURL* easyHandle);
void checkMultiInfo(CURLM* multiHandle);
void timeoutAction(CURLM* multiHandle, int* stillRunning);
size_t writeBodyCallback(void* buffer, size_t size, size_t nmemb, void* userData);
size_t writeResponseHeadersCallback(void* buffer, size_t size, size_t nmemb, void* userData);
int progressFunctionCallback(void* clientPointer, double, double, double, double);

MultiSocketLoader* s_multiSocketLoader = nullptr;
std::atomic<size_t> s_multiSocketLoaderCounter = 0;
int s_requestCounter = 0;

}

namespace CrawlerEngine
{

MultiSocketLoader::MultiSocketLoader(QObject* parent)
	: QObject(parent)
	, m_timer(nullptr)
	, m_socketPrivateData{ this, curl_multi_init(), 0 }
	, m_timeoutMilliseconds(c_second)
{
	ASSERT(s_multiSocketLoaderCounter.fetch_add(1, std::memory_order_relaxed) == 0 || !"Must be only one instance of MultiSocketLoader");
	ASSERT(m_socketPrivateData.multiHandle);

	s_multiSocketLoader = this;

	qRegisterMetaType<curl_socket_t>("curl_socket_t");

	curl_multi_setopt(m_socketPrivateData.multiHandle, CURLMOPT_SOCKETFUNCTION, socketFunctionCallback);
	curl_multi_setopt(m_socketPrivateData.multiHandle, CURLMOPT_SOCKETDATA, &m_socketPrivateData);
	curl_multi_setopt(m_socketPrivateData.multiHandle, CURLMOPT_TIMERFUNCTION, timerFunctionCallback);
	curl_multi_setopt(m_socketPrivateData.multiHandle, CURLMOPT_TIMERDATA, &m_socketPrivateData);
}

MultiSocketLoader::~MultiSocketLoader()
{
	s_multiSocketLoaderCounter.fetch_sub(1, std::memory_order_relaxed);
	s_multiSocketLoader = nullptr;
}

void MultiSocketLoader::onTimeout()
{
	timeoutAction(m_socketPrivateData.multiHandle, &m_socketPrivateData.stillRunning);
}

int MultiSocketLoader::get(const Url& url) const
{
	RequestDescriptor* request = new RequestDescriptor;
	request->easy = curl_easy_init();
	request->url = url.toDisplayString().toUtf8();

	curl_easy_setopt(request->easy, CURLOPT_CONNECTTIMEOUT, 30);

	if (m_timeoutMilliseconds)
	{
		curl_easy_setopt(request->easy, CURLOPT_TIMEOUT_MS, m_timeoutMilliseconds);
	}

	curl_easy_setopt(request->easy, CURLOPT_URL, request->url.constData());
	curl_easy_setopt(request->easy, CURLOPT_PRIVATE, request);
	curl_easy_setopt(request->easy, CURLOPT_ERRORBUFFER, request->error);
	curl_easy_setopt(request->easy, CURLOPT_FOLLOWLOCATION, 0L);
	curl_easy_setopt(request->easy, CURLOPT_SSL_VERIFYPEER, 0);

	curl_easy_setopt(request->easy, CURLOPT_WRITEFUNCTION, &writeBodyCallback);
	curl_easy_setopt(request->easy, CURLOPT_WRITEDATA, request);
	curl_easy_setopt(request->easy, CURLOPT_HEADERFUNCTION, &writeResponseHeadersCallback);
	curl_easy_setopt(request->easy, CURLOPT_HEADERDATA, request);
	curl_easy_setopt(request->easy, CURLOPT_PROGRESSFUNCTION, &progressFunctionCallback);
	curl_easy_setopt(request->easy, CURLOPT_PROGRESSDATA, &request->interruptionReason);

	applyProxySettingsIfNeeded(request->easy);

	curl_multi_add_handle(m_socketPrivateData.multiHandle, request->easy);

	request->id = s_requestCounter++;// m_socketPrivateData.stillRunning;
	request->startLoadingPoint = std::chrono::high_resolution_clock::now();

	return request->id;
}

int MultiSocketLoader::post(const Url&) const
{
	return 0;
}

int MultiSocketLoader::head(const Url& url) const
{
	RequestDescriptor* request = new RequestDescriptor;
	request->easy = curl_easy_init();
	request->url = url.toDisplayString().toUtf8();

	curl_easy_setopt(request->easy, CURLOPT_CONNECTTIMEOUT, 30);

	if (m_timeoutMilliseconds)
	{
		curl_easy_setopt(request->easy, CURLOPT_TIMEOUT_MS, m_timeoutMilliseconds);
	}

	curl_easy_setopt(request->easy, CURLOPT_URL, request->url.constData());
	curl_easy_setopt(request->easy, CURLOPT_PRIVATE, request);
	curl_easy_setopt(request->easy, CURLOPT_ERRORBUFFER, request->error);
	curl_easy_setopt(request->easy, CURLOPT_FOLLOWLOCATION, 0L);
	curl_easy_setopt(request->easy, CURLOPT_SSL_VERIFYPEER, 0);
	curl_easy_setopt(request->easy, CURLOPT_NOBODY, 1L);

	curl_easy_setopt(request->easy, CURLOPT_WRITEFUNCTION, &writeBodyCallback);
	curl_easy_setopt(request->easy, CURLOPT_WRITEDATA, request);
	curl_easy_setopt(request->easy, CURLOPT_HEADERFUNCTION, &writeResponseHeadersCallback);
	curl_easy_setopt(request->easy, CURLOPT_HEADERDATA, request);
	curl_easy_setopt(request->easy, CURLOPT_PROGRESSFUNCTION, &progressFunctionCallback);
	curl_easy_setopt(request->easy, CURLOPT_PROGRESSDATA, &request->interruptionReason);

	applyProxySettingsIfNeeded(request->easy);

	curl_multi_add_handle(m_socketPrivateData.multiHandle, request->easy);

	request->id = s_requestCounter++;// m_socketPrivateData.stillRunning;
	request->startLoadingPoint = std::chrono::high_resolution_clock::now();

	return request->id;
}

void MultiSocketLoader::setProxySettings(const QString& proxyHostName, int proxyPort, const QString& proxyUser, const QString& proxyPassword)
{
	m_proxySettings = ProxySettings
	{
		proxyHostName,
		proxyPort,
		proxyUser,
		proxyPassword
	};
}

void MultiSocketLoader::resetProxySettings()
{
	m_proxySettings = ProxySettings();
}

void MultiSocketLoader::setTimeout(size_t timeoutMilliseconds)
{
	m_timeoutMilliseconds = timeoutMilliseconds;
}

void MultiSocketLoader::resetTimeout()
{
	m_timeoutMilliseconds = 0;
}

size_t MultiSocketLoader::timeout() const
{
	return m_timeoutMilliseconds;
}

MultiSocketLoader* MultiSocketLoader::instance()
{
	return s_multiSocketLoader;
}

void MultiSocketLoader::setTimer()
{
	resetTimer();
	m_timer = new QTimer(this);
	m_timer->setInterval(1000);
	m_timer->setSingleShot(true);

	VERIFY(connect(m_timer, &QTimer::timeout, this, &MultiSocketLoader::onTimeout));

	m_timer->start();
}

void MultiSocketLoader::resetTimer()
{
	// TODO: optimize reinitialization of timer because of this method calls very frequently
	if (m_timer && m_timer->isActive())
	{
		m_timer->stop();
		m_timer->deleteLater();
		m_timer = nullptr;
	}
}

void MultiSocketLoader::applyProxySettingsIfNeeded(CURL * easyHandle) const
{
	if (!m_proxySettings.proxyHostName.isEmpty())
	{
		curl_easy_setopt(easyHandle, CURLOPT_PROXY, m_proxySettings.proxyHostName.constData());
		curl_easy_setopt(easyHandle, CURLOPT_PROXYPORT, m_proxySettings.proxyPort);

		if (!m_proxySettings.proxyUser.isEmpty() && !m_proxySettings.proxyPassword.isEmpty())
		{
			const QByteArray& userPasswordString = (m_proxySettings.proxyUser + ":" + m_proxySettings.proxyPassword).toUtf8();
			curl_easy_setopt(easyHandle, CURLOPT_PROXYUSERPWD, userPasswordString.constData());
		}
	}
}

int MultiSocketLoader::socketFunctionCallback(CURL* easy, curl_socket_t s, int action, void* userPrivateData, void* socketPrivateData)
{
	const auto setSocketInfo = [&](SocketInfo* socketInfo)
	{
		socketInfo->sockfd = s;
		socketInfo->action = action;
		socketInfo->easy = easy;
	};

	SocketFunctionCallbackPrivateData* privateData = reinterpret_cast<SocketFunctionCallbackPrivateData*>(userPrivateData);
	SocketInfo* socketInfo = reinterpret_cast<SocketInfo*>(socketPrivateData);

	if (action == CURL_POLL_REMOVE)
	{
		delete socketInfo;
	}
	else
	{
		if (!socketInfo)
		{
			socketInfo = new SocketInfo;
			setSocketInfo(socketInfo);
			curl_multi_assign(privateData->multiHandle, s, socketInfo);
		}
		else
		{
			setSocketInfo(socketInfo);
		}

		privateData->multiSocketLoader->resetTimer();

		VERIFY(QMetaObject::invokeMethod(privateData->multiSocketLoader, "doAction",
			Qt::QueuedConnection, Q_ARG(curl_socket_t, socketInfo->sockfd), Q_ARG(int, action)));
	}

	return 0;
}

int MultiSocketLoader::timerFunctionCallback(CURLM* multiHandle, long timeoutMs, void* userPrivateData)
{
	SocketFunctionCallbackPrivateData* privateData = reinterpret_cast<SocketFunctionCallbackPrivateData*>(userPrivateData);
	privateData->multiSocketLoader->resetTimer();

	if (timeoutMs > 0)
	{
		privateData->multiSocketLoader->setTimer();
	}
	else
	{
		timeoutAction(multiHandle, &privateData->stillRunning);
	}

	return 0;
}

void MultiSocketLoader::doAction(curl_socket_t socketDescriptor, int curlAction)
{
	const CURLMcode returnCode = curl_multi_socket_action(m_socketPrivateData.multiHandle,
		socketDescriptor,
		curlAction,
		&m_socketPrivateData.stillRunning);

	verifyCurlReturnCode(__FUNCTION__, returnCode);
	checkMultiInfo(m_socketPrivateData.multiHandle);

	if (m_socketPrivateData.stillRunning <= 0)
	{
		resetTimer();
	}
	else
	{
		VERIFY(QMetaObject::invokeMethod(this, "doAction", Qt::QueuedConnection,
			Q_ARG(curl_socket_t, socketDescriptor), Q_ARG(int, curlAction)));
	}
}

}

/************************ Other internal dependencies **************************/

namespace
{

void verifyCurlReturnCode(const char* where, CURLMcode code)
{
	if (CURLM_OK == code)
	{
		return;
	}

	const char* errorString = nullptr;

	switch (code)
	{
		case CURLM_CALL_MULTI_PERFORM:
		{
			errorString = "CURLM_CALL_MULTI_PERFORM";
			break;
		}
		case CURLM_BAD_HANDLE:
		{
			errorString = "CURLM_BAD_HANDLE";
			break;
		}
		case CURLM_BAD_EASY_HANDLE:
		{
			errorString = "CURLM_BAD_EASY_HANDLE";
			break;
		}
		case CURLM_OUT_OF_MEMORY:
		{
			errorString = "CURLM_OUT_OF_MEMORY";
			break;
		}
		case CURLM_INTERNAL_ERROR:
		{
			errorString = "CURLM_INTERNAL_ERROR";
			break;
		}
		case CURLM_UNKNOWN_OPTION:
		{
			errorString = "CURLM_UNKNOWN_OPTION";
			break;
		}
		case CURLM_LAST:
		{
			errorString = "CURLM_LAST";
			break;
		}
		case CURLM_BAD_SOCKET:
		{
			errorString = "CURLM_BAD_SOCKET";
			break;
		}
		default: errorString = "CURLM_unknown";
		{
			break;
		}
	}

	ERRLOG << "ERROR:" << where << "returns" << errorString;
}

long getHttpCode(CURL* easyHandle)
{
	long httpCode = 0;
	const auto result = curl_easy_getinfo(easyHandle, CURLINFO_RESPONSE_CODE, &httpCode);

	if (result != CURLE_OK)
	{
		ERRLOG
			<< "Cannot get HTTP code ("
			<< curl_easy_strerror(result)
			<< ")";
	}

	return httpCode;
}

void checkMultiInfo(CURLM* multiHandle)
{
	CURLMsg* curlMessage = nullptr;
	int messagesLeft = 0;

	while ((curlMessage = curl_multi_info_read(multiHandle, &messagesLeft)))
	{
		if (curlMessage->msg == CURLMSG_DONE)
		{
			CURL* easyHandle = curlMessage->easy_handle;
			const CURLcode result = curlMessage->data.result;

			RequestDescriptor* requestDescriptor = nullptr;
			curl_easy_getinfo(easyHandle, CURLINFO_PRIVATE, &requestDescriptor);

			if (result == CURLE_OPERATION_TIMEDOUT)
			{
				requestDescriptor->statusCode = Common::StatusCode::Timedout;
			}
			else
			{
				requestDescriptor->statusCode = static_cast<Common::StatusCode>(getHttpCode(easyHandle));
			}

			if (result != CURLE_OK &&
				result != CURLE_OPERATION_TIMEDOUT)
			{
				ERRLOG << requestDescriptor->error;
			}
			else
			{
				DEBUGLOG
					<< requestDescriptor->url
					<< "loaded"
					<< "(status code:"
					<< Common::StatusCodeDescription::description(requestDescriptor->statusCode)
					<< ")";
			}

			curl_multi_remove_handle(multiHandle, easyHandle);
			curl_easy_cleanup(easyHandle);

			MultiSocketLoader* multiSocketLoader = MultiSocketLoader::instance();
			ASSERT(multiSocketLoader);

			const std::chrono::time_point<std::chrono::high_resolution_clock> endPointLoading =
				std::chrono::high_resolution_clock::now();

			const int timeElapsed = std::chrono::duration_cast<std::chrono::milliseconds>(endPointLoading - requestDescriptor->startLoadingPoint).count();

			emit multiSocketLoader->loaded(requestDescriptor->id,
				requestDescriptor->url,
				requestDescriptor->data,
				requestDescriptor->responseHeaders,
				requestDescriptor->statusCode,
				timeElapsed);

			delete requestDescriptor;
		}
	}
}

void timeoutAction(CURLM* multiHandle, int* stillRunning)
{
	const CURLMcode returnCode = curl_multi_socket_action(multiHandle, CURL_SOCKET_TIMEOUT, 0, stillRunning);

	verifyCurlReturnCode(__FUNCTION__, returnCode);

	checkMultiInfo(multiHandle);
}

size_t writeBodyCallback(void* buffer, size_t size, size_t nmemb, void* userData)
{
	RequestDescriptor* connectionInfo = reinterpret_cast<RequestDescriptor*>(userData);
	const auto bufferSize = size * nmemb;
	connectionInfo->data.append(reinterpret_cast<const char*>(buffer), static_cast<int>(bufferSize));
	return bufferSize;
}

size_t writeResponseHeadersCallback(void* buffer, size_t size, size_t nmemb, void* userData)
{
	RequestDescriptor* requestDescriptor = reinterpret_cast<RequestDescriptor*>(userData);
	requestDescriptor->responseHeaders.addHeaderValue(reinterpret_cast<const char*>(buffer));

	const std::vector<QString> value = requestDescriptor->responseHeaders.valueOf("content-type");

	// by default we should to download body only of HTML resources
	if (!value.empty() && !PageParserHelpers::isHtmlOrPlainContentType(value[0]))
	{
		requestDescriptor->interruptionReason = RequestDescriptor::InterruptionReason::ReasonNonHtmlResponse;
	}

	return size * nmemb;
}

int progressFunctionCallback(void* clientPointer, double, double, double, double)
{
	const RequestDescriptor::InterruptionReason* interruptionReason = reinterpret_cast<RequestDescriptor::InterruptionReason*>(clientPointer);

	if (*interruptionReason == RequestDescriptor::InterruptionReason::ReasonNonHtmlResponse)
	{
		// abort
		return 1;
	}

	// continue loading
	return 0;
}

}