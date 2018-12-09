#include "multi_socket_download_handler.h"
#include "random_interval_range_timer.h"
#include "multi_socket_loader.h"
#include "download_request.h"
#include "download_response.h"
#include "helpers.h"
#include "thread_message_dispatcher.h"

namespace
{

constexpr int c_maxParallelTransferCount = 24;

}

namespace CrawlerEngine
{

MultiSocketDownloadHandler::MultiSocketDownloadHandler()
	: m_multiSocketLoader(new MultiSocketLoader(this))
{
	qRegisterMetaType<ResponseHeaders>("ResponseHeaders");
	qRegisterMetaType<Common::StatusCode>("Common::StatusCode");

	// must be queued connection
	// otherwise loader can emit signal about loaded url
	// before we add the request id to the m_requesters map
	// in this case we have not a chance to find a requester in the onUrlLoaded slot!!!
	VERIFY(connect(m_multiSocketLoader, &MultiSocketLoader::loaded,
		this, &MultiSocketDownloadHandler::onUrlLoaded, Qt::QueuedConnection));

	VERIFY(connect(m_multiSocketLoader, &MultiSocketLoader::currentParallelTransferCountChanged,
		this, &MultiSocketDownloadHandler::onCurrentParallelTransfersCountChanged));

	//
	// TODO: optimize the many calls by these connections
	//

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

void MultiSocketDownloadHandler::pauseRequesters(const QVector<const void*>& requesterToBePaused)
{
	std::for_each(requesterToBePaused.begin(), requesterToBePaused.end(), [this](const void* p)
	{
		m_pausedRequesters.insert(p);
	});
}

void MultiSocketDownloadHandler::unpauseRequesters(const QVector<const void*>& requesterToBeUnpaused)
{
	std::for_each(requesterToBeUnpaused.begin(), requesterToBeUnpaused.end(), [this](const void* p)
	{
		m_pausedRequesters.remove(p);
	});
}

Url MultiSocketDownloadHandler::redirectedUrl(const ResponseHeaders& responseHeaders, const Url& baseAddress) const
{
	const std::vector<QString> locationHeaderData = responseHeaders.valueOf("location");

	Url redirectAddress;

	// supposed that we've got only one location header
	// so we use locationHeaderData[0] to get the redirect url
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
	const auto requesterIterator = m_activeRequesters.find(id);

	if (requesterIterator == m_activeRequesters.end())
	{
		const int parentId = parentIdFor(id);

		DEBUG_ASSERT(parentId != -1 || !"Parent ID not found");

		return m_activeRequesters[parentId].lock();
	}

	return requesterIterator.value().lock();
}

RequesterSharedPtr MultiSocketDownloadHandler::requesterById(int id) const
{
	const auto requesterIterator = m_activeRequesters.find(id);

	if (requesterIterator == m_activeRequesters.end())
	{
		const int parentId = parentIdFor(id);

		if (parentId == -1)
		{
			return RequesterSharedPtr();
		}

		return m_activeRequesters[parentId].lock();
	}

	return requesterIterator.value().lock();
}

int MultiSocketDownloadHandler::parentIdFor(int id) const
{
	int parendIdSearchResult = -1;

	auto redirectBindingIterator = m_redirectRequestIdToParentId.find(id);

	while (redirectBindingIterator != m_redirectRequestIdToParentId.end())
	{
		parendIdSearchResult = redirectBindingIterator.value();
		redirectBindingIterator = m_redirectRequestIdToParentId.find(parendIdSearchResult);
	}

	return parendIdSearchResult;
}

void MultiSocketDownloadHandler::removeRequestIndexesChain(int id)
{
	std::vector<int> chain;

	auto redirectBindingIterator = m_redirectRequestIdToParentId.find(id);

	while (redirectBindingIterator != m_redirectRequestIdToParentId.end())
	{
		chain.push_back(redirectBindingIterator.key());
		redirectBindingIterator = m_redirectRequestIdToParentId.find(redirectBindingIterator.value());
	}
}

void MultiSocketDownloadHandler::followLocation(DownloadRequest::BodyProcessingCommand bodyProcessingCommand,
	int parentRequestId,
	const Url& redirectUrlAddress,
	DownloadRequestType requestType)
{
	const CrawlerRequest redirectKey{ redirectUrlAddress, requestType };
	const int redirectionRequestId = loadHelper(redirectKey, bodyProcessingCommand);
	m_redirectRequestIdToParentId[redirectionRequestId] = parentRequestId;
}


bool MultiSocketDownloadHandler::isRedirectLoop(const HopsChain& hopsChain, const Url& redirectUrlAddress) const
{
	const auto urlComparator = [&redirectUrlAddress](const Hop& hop)
	{
		return hop.url() == redirectUrlAddress;
	};

	const int urlCount = std::count_if(hopsChain.begin(), hopsChain.end(), urlComparator);

	return urlCount > 2;
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
	DEBUGLOG << "Loaded: " << url;

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

	response->hopsChain.addHop(Hop(loadedResourceUrl, redirectUrlAddress, statusCode, data, responseHeaders, timeElapsed));

	if (isRedirectionStatusCode)
	{
		DEBUGLOG
			<< "Url: "
			<< url
			<< " was redirected to: "
			<< redirectUrlAddress.urlStr();

		if (response->hopsChain.length() == static_cast<size_t>(maxRedirectsToProcess()))
		{
			statusCode = Common::StatusCode::TooManyRedirections;
		}
		else if (isRedirectLoop(response->hopsChain, redirectUrlAddress))
		{
			statusCode = Common::StatusCode::RedirectLoop;

			DEBUGLOG
				<< "redirect loop detected"
				<< url;
		}
		else
		{
			followLocation(request->bodyProcessingCommand, id, redirectUrlAddress, requestType);
			return;
		}
	}

	ThreadMessageDispatcher::forThread(requester->thread())->postResponse(requester, response);

	const int parentRequestId = parentIdFor(id);
	const int idToDelete = parentRequestId == -1 ? id : parentRequestId;

	m_responses.remove(idToDelete);
	m_activeRequesters.remove(idToDelete);

	removeRequestIndexesChain(id);
}


void MultiSocketDownloadHandler::onCurrentParallelTransfersCountChanged(int count)
{
	if (count >= c_maxParallelTransferCount)
	{
		return;
	}

	if (m_pendingRequesters.isEmpty())
	{
		return;
	}

	RequesterSharedPtr requester = m_pendingRequesters.dequeue().lock();

	if (!requester)
	{
		return;
	}

	if (m_pausedRequesters.contains(requester.get()))
	{

		return;
	}

	load(requester);
}

void MultiSocketDownloadHandler::load(RequesterSharedPtr requester)
{
	if (m_multiSocketLoader->currentParallelConnections() >= c_maxParallelTransferCount)
	{
		m_pendingRequesters.enqueue(requester);
		return;
	}

	const DownloadRequest* request = Common::Helpers::fast_cast<DownloadRequest*>(requester->request());

	const int requestId = loadHelper(request->requestInfo, request->bodyProcessingCommand);

	// we add requester to the m_requesters only for first request
	// redirect requests here will not be stored
	m_activeRequesters[requestId] = requester;
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

	return requestId;
}

}
