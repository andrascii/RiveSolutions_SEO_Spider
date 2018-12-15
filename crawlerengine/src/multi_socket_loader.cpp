#include "multi_socket_loader.h"
#include "page_parser_helpers.h"
#include "status_code_description.h"
#include "hops_chain.h"

namespace
{

using namespace CrawlerEngine;

constexpr size_t c_second = 1000;

// used to store an action should be done by curl_multi_socket_action
struct SocketInfo
{
	curl_socket_t sockfd;
	CURL* easy;
	int action;
};

std::atomic<size_t> s_multiSocketLoaderCounter = 0;

long getHttpCode(CURL* easyHandle);
void verifyCurlReturnCode(const char* where, CURLMcode code);
void checkMultiInfo(CURLM* multiHandle, MultiSocketLoader* multiSocketLoader);
void timeoutAction(CURLM* multiHandle, int* stillRunning, MultiSocketLoader* multiSocketLoader);

void notifyAboutTransferProgress(MultiSocketLoader* multiSocketLoader,
	const RequestDescriptor* requestDescriptor,
	double downloadTotal,
	double downloadReceived,
	double uploadTotal,
	double uploadSent);

}

namespace CrawlerEngine
{

MultiSocketLoader* MultiSocketLoader::s_instance = nullptr;

MultiSocketLoader::MultiSocketLoader(QObject* parent)
	: QObject(parent)
	, m_timer(nullptr)
	, m_socketPrivateData{ this, curl_multi_init(), 0 }
	, m_timeoutMilliseconds(0)
	, m_requestCounter(0)
	, m_timerId(0)
{
	ASSERT(s_multiSocketLoaderCounter.fetch_add(1, std::memory_order_relaxed) == 0 || !"Must be only one instance of MultiSocketLoader");
	ASSERT(m_socketPrivateData.multiHandle);

	s_instance = this;

	qRegisterMetaType<curl_socket_t>("curl_socket_t");

	curl_multi_setopt(m_socketPrivateData.multiHandle, CURLMOPT_SOCKETFUNCTION, socketFunctionCallback);
	curl_multi_setopt(m_socketPrivateData.multiHandle, CURLMOPT_SOCKETDATA, &m_socketPrivateData);
	curl_multi_setopt(m_socketPrivateData.multiHandle, CURLMOPT_TIMERFUNCTION, timerFunctionCallback);
	curl_multi_setopt(m_socketPrivateData.multiHandle, CURLMOPT_TIMERDATA, &m_socketPrivateData);
}

MultiSocketLoader::~MultiSocketLoader()
{
	s_multiSocketLoaderCounter.fetch_sub(1, std::memory_order_relaxed);
	s_instance = nullptr;
}

int MultiSocketLoader::get(const Url& url, DownloadRequest::BodyProcessingCommand bodyProcessingCommand)
{
	RequestDescriptor* request = createRequestDescriptor(url, m_timeoutMilliseconds, m_headers.get());
	applyProxySettingsIfNeeded(request->easy);

	request->id = m_requestCounter++;
	request->startLoadingPoint = std::chrono::high_resolution_clock::now();
	request->bodyProcessingCommand = bodyProcessingCommand;
	request->method = RequestDescriptor::Method::Get;

	// we should to return a copy of the request id
	// because of curl_multi_add_handle immediately invokes
	// checkMultiInfo which may to delete our request pointer
	// in this case we will access to the freed memory - Undefined Behavior
	const int returnValue = request->id;

	m_activeRequestDescriptors[returnValue] = request;
	curl_multi_add_handle(m_socketPrivateData.multiHandle, request->easy);

	return returnValue;
}

int MultiSocketLoader::post(const Url& url, QByteArray uploadData)
{
	RequestDescriptor* request = createRequestDescriptor(url, m_timeoutMilliseconds, m_headers.get());
	request->uploadData.buffer = std::move(uploadData);
	request->uploadData.dataPointer = request->uploadData.buffer.constData();
	request->uploadData.dataLength = request->uploadData.buffer.size();

	applyProxySettingsIfNeeded(request->easy);
	request->id = m_requestCounter++;
	request->startLoadingPoint = std::chrono::high_resolution_clock::now();
	request->method = RequestDescriptor::Method::Post;

	curl_easy_setopt(request->easy, CURLOPT_POST, 1L);
	curl_easy_setopt(request->easy, CURLOPT_POSTFIELDSIZE, request->uploadData.buffer.size());
	curl_easy_setopt(request->easy, CURLOPT_READFUNCTION, readUploadDataCallback);
	curl_easy_setopt(request->easy, CURLOPT_READDATA, &request->uploadData);

	// we should to return a copy of the request id
	// because of curl_multi_add_handle immediately invokes
	// checkMultiInfo which may to delete our request pointer
	// in this case we will access to the freed memory - Undefined Behavior
	const int returnValue = request->id;

	m_activeRequestDescriptors[returnValue] = request;
	curl_multi_add_handle(m_socketPrivateData.multiHandle, request->easy);

	return returnValue;
}

int MultiSocketLoader::head(const Url& url)
{
	RequestDescriptor* request = createRequestDescriptor(url, m_timeoutMilliseconds, m_headers.get());
	applyProxySettingsIfNeeded(request->easy);

	request->id = m_requestCounter++;
	request->startLoadingPoint = std::chrono::high_resolution_clock::now();
	request->method = RequestDescriptor::Method::Head;

	curl_easy_setopt(request->easy, CURLOPT_NOBODY, 1L);

	// we should to return a copy of the request id
	// because of curl_multi_add_handle immediately invokes
	// checkMultiInfo which may to delete our request pointer
	// in this case we will access to the freed memory - Undefined Behavior
	const int returnValue = request->id;

	m_activeRequestDescriptors[returnValue] = request;
	curl_multi_add_handle(m_socketPrivateData.multiHandle, request->easy);

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

void MultiSocketLoader::setUserAgent(const QByteArray& userAgent)
{
	m_headers.add("User-Agent", userAgent);
}

int MultiSocketLoader::currentParallelConnections() const noexcept
{
	return m_socketPrivateData.stillRunning;
}

void MultiSocketLoader::pauseConnection(int id) const
{
	if (!m_activeRequestDescriptors.contains(id))
	{
		return;
	}

	curl_easy_pause(m_activeRequestDescriptors[id]->easy, CURLPAUSE_ALL);
}

void MultiSocketLoader::unpauseConnection(int id)
{
	if (!m_activeRequestDescriptors.contains(id))
	{
		return;
	}

	curl_easy_pause(m_activeRequestDescriptors[id]->easy, CURLPAUSE_CONT);
	curl_multi_socket_all(m_socketPrivateData.multiHandle, &m_socketPrivateData.stillRunning);
}

void MultiSocketLoader::resetConnection(int id)
{
	if (!m_activeRequestDescriptors.contains(id))
	{
		return;
	}

	curl_multi_remove_handle(m_socketPrivateData.multiHandle, m_activeRequestDescriptors[id]->easy);
	curl_easy_cleanup(m_activeRequestDescriptors[id]->easy);
}

void MultiSocketLoader::setTimer()
{
	resetTimer();
	m_timerId = startTimer(c_second);
	ASSERT(m_timerId);
}

void MultiSocketLoader::resetTimer()
{
	if (!m_timerId)
	{
		return;
	}

	killTimer(m_timerId);
	m_timerId = 0;
}

void MultiSocketLoader::applyProxySettingsIfNeeded(CURL* easyHandle) const
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

MultiSocketLoader* MultiSocketLoader::instance()
{
	return s_instance;
}

RequestDescriptor* MultiSocketLoader::createRequestDescriptor(const Url& url, size_t timeoutMilliseconds, curl_slist* headers)
{
	const auto requestIdDeleter = [this](int id)
	{
		m_activeRequestDescriptors.remove(id);
	};

	RequestDescriptor* requestDescriptor = new RequestDescriptor(requestIdDeleter);
	requestDescriptor->easy = curl_easy_init();
	requestDescriptor->url = url.toDisplayString().toUtf8();

	curl_easy_setopt(requestDescriptor->easy, CURLOPT_CONNECTTIMEOUT, 30);

	if (timeoutMilliseconds)
	{
		curl_easy_setopt(requestDescriptor->easy, CURLOPT_TIMEOUT_MS, timeoutMilliseconds);
	}

	curl_easy_setopt(requestDescriptor->easy, CURLOPT_URL, requestDescriptor->url.constData());
	curl_easy_setopt(requestDescriptor->easy, CURLOPT_PRIVATE, requestDescriptor);
	curl_easy_setopt(requestDescriptor->easy, CURLOPT_ERRORBUFFER, requestDescriptor->error);
	curl_easy_setopt(requestDescriptor->easy, CURLOPT_FOLLOWLOCATION, 0L);
	curl_easy_setopt(requestDescriptor->easy, CURLOPT_SSL_VERIFYPEER, 0);
	curl_easy_setopt(requestDescriptor->easy, CURLOPT_NOPROGRESS, 0L);

	curl_easy_setopt(requestDescriptor->easy, CURLOPT_WRITEFUNCTION, MultiSocketLoader::writeBodyCallback);
	curl_easy_setopt(requestDescriptor->easy, CURLOPT_WRITEDATA, requestDescriptor);
	curl_easy_setopt(requestDescriptor->easy, CURLOPT_HEADERFUNCTION, MultiSocketLoader::writeResponseHeadersCallback);
	curl_easy_setopt(requestDescriptor->easy, CURLOPT_HEADERDATA, requestDescriptor);
	curl_easy_setopt(requestDescriptor->easy, CURLOPT_PROGRESSFUNCTION, MultiSocketLoader::progressFunctionCallback);
	curl_easy_setopt(requestDescriptor->easy, CURLOPT_PROGRESSDATA, requestDescriptor);
	curl_easy_setopt(requestDescriptor->easy, CURLOPT_HTTPHEADER, headers);

	return requestDescriptor;
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
		timeoutAction(multiHandle, &privateData->stillRunning, privateData->multiSocketLoader);
	}

	return 0;
}

size_t MultiSocketLoader::writeBodyCallback(void* buffer, size_t size, size_t nmemb, void* userData)
{
	RequestDescriptor* connectionInfo = reinterpret_cast<RequestDescriptor*>(userData);
	const auto bufferSize = size * nmemb;
	connectionInfo->body.append(reinterpret_cast<const char*>(buffer), static_cast<int>(bufferSize));
	return bufferSize;
}

size_t MultiSocketLoader::readUploadDataCallback(void* buffer, size_t size, size_t nmemb, void* userData)
{
	RequestDescriptor::UploadData* uploadData = reinterpret_cast<RequestDescriptor::UploadData*>(userData);

	const auto bufferSize = size * nmemb;
	const auto copySize = uploadData->dataLength < bufferSize ? uploadData->dataLength : bufferSize;

	std::copy(uploadData->dataPointer, uploadData->dataPointer + copySize,
		reinterpret_cast<char*>(buffer));

	uploadData->dataLength -= copySize;
	uploadData->dataPointer += copySize;

	return copySize;
}

size_t MultiSocketLoader::writeResponseHeadersCallback(void* buffer, size_t size, size_t nmemb, void* userData)
{
	RequestDescriptor* requestDescriptor = reinterpret_cast<RequestDescriptor*>(userData);
	requestDescriptor->responseHeaders.addHeaderValue(reinterpret_cast<const char*>(buffer));

	const std::vector<QString> value = requestDescriptor->responseHeaders.valueOf("content-type");

	if (!value.empty() &&
		requestDescriptor->bodyProcessingCommand == DownloadRequest::BodyProcessingCommand::CommandAutoDetectionBodyLoading &&
		!PageParserHelpers::isHtmlOrPlainContentType(value[0]))
	{
		requestDescriptor->interruptionReason = RequestDescriptor::InterruptionReason::ReasonNonHtmlResponse;
	}

	return size * nmemb;
}

int MultiSocketLoader::progressFunctionCallback(void* clientPointer, double downloadTotal, double downloadReceived, double uploadTotal, double uploadSent)
{
	const RequestDescriptor* requestDescriptor = reinterpret_cast<RequestDescriptor*>(clientPointer);
	const bool mustBeInterrupted = requestDescriptor->interruptionReason == RequestDescriptor::InterruptionReason::ReasonNonHtmlResponse;

	MultiSocketLoader* multiSocketLoader = MultiSocketLoader::instance();

	if (mustBeInterrupted)
	{
		// abort transfer the data
		// and send transfer progress as a completed
		notifyAboutTransferProgress(multiSocketLoader, requestDescriptor, downloadReceived, downloadReceived, uploadSent, uploadSent);
		return 1;
	}

	notifyAboutTransferProgress(multiSocketLoader, requestDescriptor, downloadTotal, downloadReceived, uploadTotal, uploadSent);

	// continue transfer the data
	return 0;
}

void MultiSocketLoader::timerEvent(QTimerEvent*)
{
	resetTimer();
	timeoutAction(m_socketPrivateData.multiHandle, &m_socketPrivateData.stillRunning, this);
}

void MultiSocketLoader::doAction(curl_socket_t socketDescriptor, int curlAction)
{
	const int currentParallelConnectionCount = m_socketPrivateData.stillRunning;

	const CURLMcode returnCode = curl_multi_socket_action(m_socketPrivateData.multiHandle,
		socketDescriptor,
		curlAction,
		&m_socketPrivateData.stillRunning);

	if (currentParallelConnectionCount != m_socketPrivateData.stillRunning)
	{
		emit currentParallelTransferCountChanged(m_socketPrivateData.stillRunning);
	}

	verifyCurlReturnCode(__FUNCTION__, returnCode);
	checkMultiInfo(m_socketPrivateData.multiHandle, this);

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

void checkMultiInfo(CURLM* multiHandle, MultiSocketLoader* multiSocketLoader)
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

			DEBUG_ASSERT(requestDescriptor->id != -1 || !"id is invalid");

			if (result == CURLE_OPERATION_TIMEDOUT)
			{
				requestDescriptor->statusCode = Common::StatusCode::Timeout;
			}
			else
			{
				requestDescriptor->statusCode = static_cast<Common::StatusCode>(getHttpCode(easyHandle));
			}

			if (result != CURLE_OK &&
				result != CURLE_OPERATION_TIMEDOUT &&
				result != CURLE_ABORTED_BY_CALLBACK)
			{
				ERRLOG
					<< "Url: "
					<< requestDescriptor->url
					<< " ID: "
					<< requestDescriptor->id
					<< " error: "
					<< requestDescriptor->error;
			}
			else
			{
				DEBUGLOG
					<< requestDescriptor->url
					<< " loaded "
					<< "(status code: "
					<< Common::StatusCodeDescription::description(requestDescriptor->statusCode)
					<< ", id: " << requestDescriptor->id << ")";
			}

			DEBUG_ASSERT(requestDescriptor->method != RequestDescriptor::Method::Head || requestDescriptor->body.isEmpty());

			curl_multi_remove_handle(multiHandle, easyHandle);
			curl_easy_cleanup(easyHandle);

			const std::chrono::time_point<std::chrono::high_resolution_clock> endPointLoading =
				std::chrono::high_resolution_clock::now();

			const int timeElapsed = std::chrono::duration_cast<std::chrono::milliseconds>(endPointLoading - requestDescriptor->startLoadingPoint).count();

			emit multiSocketLoader->loaded(requestDescriptor->id,
				requestDescriptor->url,
				requestDescriptor->body,
				requestDescriptor->responseHeaders,
				requestDescriptor->statusCode,
				timeElapsed);

			delete requestDescriptor;
		}
	}
}

void timeoutAction(CURLM* multiHandle, int* stillRunning, MultiSocketLoader* multiSocketLoader)
{
	const CURLMcode returnCode = curl_multi_socket_action(multiHandle, CURL_SOCKET_TIMEOUT, 0, stillRunning);

	verifyCurlReturnCode(__FUNCTION__, returnCode);

	checkMultiInfo(multiHandle, multiSocketLoader);
}

void notifyAboutTransferProgress(MultiSocketLoader* multiSocketLoader,
	const RequestDescriptor* requestDescriptor,
	double downloadTotal,
	double downloadReceived,
	double uploadTotal,
	double uploadSent)
{
	constexpr int pause = 300;

	static std::chrono::time_point<std::chrono::high_resolution_clock> lastEmitTimePoint =
		std::chrono::high_resolution_clock::now();

	const int timePassed = std::chrono::duration_cast<std::chrono::milliseconds>(
		std::chrono::high_resolution_clock::now() - lastEmitTimePoint).count();

	if (timePassed < pause)
	{
		return;
	}

	ASSERT(multiSocketLoader);

	if (requestDescriptor->method == RequestDescriptor::Method::Get ||
		requestDescriptor->method == RequestDescriptor::Method::Head)
	{
		emit multiSocketLoader->downloadProgress(requestDescriptor->id, downloadTotal, downloadReceived);
	}

	if (requestDescriptor->method == RequestDescriptor::Method::Post)
	{
		emit multiSocketLoader->uploadProgress(requestDescriptor->id, uploadTotal, uploadSent);
	}

	lastEmitTimePoint = std::chrono::high_resolution_clock::now();
}

}