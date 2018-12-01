#include "multi_socket_download_handler.h"
#include "random_interval_range_timer.h"
#include "handler_registry.h"
#include "multi_socket_loader.h"
#include "download_request.h"
#include "download_response.h"
#include "helpers.h"
#include "thread_message_dispatcher.h"

namespace CrawlerEngine
{

MultiSocketDownloadHandler::MultiSocketDownloadHandler()
	: m_multiSocketLoader(new MultiSocketLoader(this))
{
	HandlerRegistry& handlerRegistry = HandlerRegistry::instance();
	handlerRegistry.registrateHandler(this, RequestType::RequestDownload);

	VERIFY(connect(m_multiSocketLoader, &MultiSocketLoader::loaded,
		this, &MultiSocketDownloadHandler::onUrlLoaded, Qt::DirectConnection));

//	VERIFY(connect(m_multiSocketLoader, &MultiSocketLoader::uploadProgress,
//		this, &MultiSocketDownloadHandler::onAboutUploadProgress, Qt::QueuedConnection));
//
//	VERIFY(connect(m_multiSocketLoader, &MultiSocketLoader::downloadProgress,
//		this, &MultiSocketDownloadHandler::onAboutDownloadProgress, Qt::QueuedConnection));
}

void MultiSocketDownloadHandler::setUserAgent(const QByteArray& userAgent)
{
	m_multiSocketLoader->setUserAgent(userAgent);
}

void MultiSocketDownloadHandler::setProxy(const QString& proxyHostName, int proxyPort, const QString& proxyUser, const QString& proxyPassword)
{
	m_multiSocketLoader->setProxySettings(proxyHostName, proxyPort, proxyUser, proxyPassword);
}

void MultiSocketDownloadHandler::resetProxy()
{
	m_multiSocketLoader->resetProxySettings();
}

void MultiSocketDownloadHandler::setTimeout(int msecs)
{
	m_multiSocketLoader->setTimeout(msecs);
}

void MultiSocketDownloadHandler::stopRequestHandling(RequesterSharedPtr requester)
{
	return void();
}

QObject* MultiSocketDownloadHandler::qobject()
{
	return this;
}

std::shared_ptr<DownloadResponse> MultiSocketDownloadHandler::responseFor(int requestId)
{
	if (!m_responses.contains(requestId))
	{
		const int parentId = parentIdFor(requestId);

		if (!m_responses.contains(parentId))
		{
			m_responses[requestId] = std::make_shared<DownloadResponse>();
		}
		else
		{
			return m_responses[parentId];
		}
	}

	return m_responses[requestId];
}

Url MultiSocketDownloadHandler::redirectedUrl(const ResponseHeaders& responseHeaders, const Url& baseAddress) const
{
	const std::vector<QString> locationHeaderData = responseHeaders.valueOf("location");

	Url redirectAddress;

	// supposed that we've got only one location header
	// so we use locationHeaderData[0] to get the refirect url
	if (!locationHeaderData.empty())
	{
		redirectAddress = locationHeaderData[0];

		if (redirectAddress.isValid() && redirectAddress.isRelative())
		{
			redirectAddress = PageParserHelpers::resolveRelativeUrl(redirectAddress, baseAddress);
		}
	}

	return redirectAddress;
}

RequesterSharedPtr MultiSocketDownloadHandler::requesterByIdAssertIfNotExists(int id) const
{
	const auto requesterIterator = m_requesters.find(id);

	if (requesterIterator == m_requesters.end())
	{
		const int parentId = parentIdFor(id);

		DEBUG_ASSERT(parentId != -1 || !"Parent ID not found");

		return m_requesters[parentId].lock();
	}

	return requesterIterator.value().lock();
}

RequesterSharedPtr MultiSocketDownloadHandler::requesterById(int id) const
{
	const auto requesterIterator = m_requesters.find(id);

	if (requesterIterator == m_requesters.end())
	{
		const int parentId = parentIdFor(id);

		if (parentId == -1)
		{
			return RequesterSharedPtr();
		}

		return m_requesters[parentId].lock();
	}

	return requesterIterator.value().lock();
}

int MultiSocketDownloadHandler::parentIdFor(int id) const
{
	int parendIdSearchResult = -1;

	auto redirectBindingIterator = m_idBindings.redirectRequestIdToParentId.find(id);

	while (redirectBindingIterator != m_idBindings.redirectRequestIdToParentId.end())
	{
		parendIdSearchResult = redirectBindingIterator.value();
		redirectBindingIterator = m_idBindings.redirectRequestIdToParentId.find(parendIdSearchResult);
	}

	return parendIdSearchResult;
}

void MultiSocketDownloadHandler::followLocation(DownloadRequest::BodyProcessingCommand bodyProcessingCommand,
	const std::shared_ptr<DownloadResponse>& response,
	int parentRequestId,
	const Url& url,
	const Url& redirectUrlAddress,
	const QByteArray& data,
	DownloadRequestType requestType,
	Common::StatusCode statusCode,
	const ResponseHeaders& responseHeaders,
	int timeElapsed)
{
	int urlsInChain = 0;

	for (size_t i = 0; i < response->hopsChain.length(); ++i)
	{
		if (response->hopsChain[i].url() == redirectUrlAddress)
		{
			urlsInChain += 1;
		}
	}

	if (urlsInChain <= 2)
	{
		const CrawlerRequest redirectKey{ redirectUrlAddress, requestType };
		const int redirectionRequestId = loadHelper(redirectKey, bodyProcessingCommand);

		m_idBindings.redirectRequestIdToParentId[redirectionRequestId] = parentRequestId;
		m_idBindings.parentIdToRedirectRequestId[parentRequestId].append(redirectionRequestId);

		response->hopsChain.addHop(Hop(url, redirectUrlAddress, statusCode, data, responseHeaders, timeElapsed));
	}
}

void MultiSocketDownloadHandler::onAboutDownloadProgress(int id, double bytesTotal, double bytesReceived)
{
	const RequesterSharedPtr requester = requesterById(id);

	if (!requester)
	{
		return;
	}

	const std::shared_ptr<DownloadProgressResponse> downloadProgressResponse =
		std::make_shared<DownloadProgressResponse>();

	downloadProgressResponse->bytesReceived = static_cast<quint64>(bytesReceived);
	downloadProgressResponse->bytesTotal = static_cast<quint64>(bytesTotal);

	ThreadMessageDispatcher::forThread(requester->thread())->postResponse(requester, downloadProgressResponse);
}

void MultiSocketDownloadHandler::onAboutUploadProgress(int id, double bytesTotal, double bytesSent)
{
	const RequesterSharedPtr requester = requesterById(id);

	if (!requester)
	{
		return;
	}

	const std::shared_ptr<UploadProgressResponse> uploadProgressResponse =
		std::make_shared<UploadProgressResponse>();

	uploadProgressResponse->bytesSent = static_cast<quint64>(bytesSent);
	uploadProgressResponse->bytesTotal = static_cast<quint64>(bytesTotal);

	ThreadMessageDispatcher::forThread(requester->thread())->postResponse(requester, uploadProgressResponse);
}

void MultiSocketDownloadHandler::onUrlLoaded(int id,
	const QByteArray& url,
	const QByteArray& data,
	const ResponseHeaders& responseHeaders,
	Common::StatusCode statusCode,
	int timeElapsed)
{
	DEBUGLOG << "Loaded:" << url;

	if (statusCode == Common::StatusCode::ProxyAuthenticationRequired407)
	{
		proxyAuthenticationRequired();
	}

	const RequesterSharedPtr requester = requesterByIdAssertIfNotExists(id);

	if (!requester)
	{
		return;
	}

	const DownloadRequest* request = Common::Helpers::fast_cast<DownloadRequest*>(requester->request());
	const DownloadRequestType requestType = request->requestInfo.requestType;

	const bool isRedirectionStatusCode =
		statusCode == Common::StatusCode::MovedPermanently301 ||
		statusCode == Common::StatusCode::MovedTemporarily302;

	const std::shared_ptr<DownloadResponse> response = responseFor(id);

	const Url loadedResourceUrl(url);
	const Url redirectUrlAddress = redirectedUrl(responseHeaders, loadedResourceUrl);

	if (isRedirectionStatusCode)
	{
		if (response->hopsChain.length() == static_cast<size_t>(maxRedirectsToProcess()))
		{
			statusCode = Common::StatusCode::TooManyRedirections;
		}
		else
		{
			followLocation(request->bodyProcessingCommand,
				response,
				id,
				loadedResourceUrl,
				redirectUrlAddress,
				data,
				requestType,
				statusCode,
				responseHeaders,
				timeElapsed);

			return;
		}
	}

	response->hopsChain.addHop(Hop(loadedResourceUrl, redirectUrlAddress, statusCode, data, responseHeaders, timeElapsed));
	ThreadMessageDispatcher::forThread(requester->thread())->postResponse(requester, response);

	m_responses.remove(id);
	m_requesters.remove(id);

	foreach(int redirectRequestId, m_idBindings.parentIdToRedirectRequestId[id])
	{
		m_idBindings.redirectRequestIdToParentId.remove(redirectRequestId);
	}

	m_idBindings.parentIdToRedirectRequestId.remove(id);

	ASSERT(m_activeRequests.removeOne(id));
}

void MultiSocketDownloadHandler::load(RequesterSharedPtr requester)
{
	const DownloadRequest* request = Common::Helpers::fast_cast<DownloadRequest*>(requester->request());

	const int requestId = loadHelper(request->requestInfo, request->bodyProcessingCommand);

	m_requesters[requestId] = requester;
}

int MultiSocketDownloadHandler::loadHelper(const CrawlerRequest& request, DownloadRequest::BodyProcessingCommand bodyProcessingCommand)
{
	int requestId = 0;

	switch (request.requestType)
	{
		case DownloadRequestType::RequestTypeGet:
		{
			requestId = m_multiSocketLoader->get(request.url, bodyProcessingCommand);
			break;
		}
		case DownloadRequestType::RequestTypeHead:
		{
			requestId = m_multiSocketLoader->head(request.url);
			break;
		}
		default:
		{
			ASSERT(!"Unsupported request type");
			break;
		}
	}

	m_activeRequests.push_back(requestId);

	return requestId;
}

}
