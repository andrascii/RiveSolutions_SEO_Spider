#include "multi_socket_download_handler.h"
#include "service_locator.h"
#include "inotification_service.h"
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
	: QObject(nullptr)
	, m_multiSocketLoader(new MultiSocketLoader(this))
	, m_randomIntervalRangeTimer(new RandomIntervalRangeTimer(this))
	, m_maxRedirects(-1)
	, m_timeout(-1)
{
	HandlerRegistry& handlerRegistry = HandlerRegistry::instance();
	handlerRegistry.registrateHandler(this, RequestType::RequestTypeDownload);

	VERIFY(connect(m_randomIntervalRangeTimer, &RandomIntervalRangeTimer::timerTicked,
		this, &MultiSocketDownloadHandler::onTimerTicked, Qt::DirectConnection));

	VERIFY(connect(m_multiSocketLoader, &MultiSocketLoader::loaded,
		this, &MultiSocketDownloadHandler::onUrlLoaded, Qt::DirectConnection));
}

void MultiSocketDownloadHandler::setPauseRange(int from, int to)
{
	resetPauseRange();
	m_randomIntervalRangeTimer->setRange(from, to);
	m_randomIntervalRangeTimer->start();
}

void MultiSocketDownloadHandler::resetPauseRange()
{
	DEBUG_ASSERT(thread() == QThread::currentThread() && "This method should be called from the same thread");
	m_randomIntervalRangeTimer->reset();
}

void MultiSocketDownloadHandler::setTimeout(int msecs)
{
	m_multiSocketLoader->setTimeout(msecs);
}

void MultiSocketDownloadHandler::setMaxRedirects(int redirects)
{
	m_maxRedirects = redirects;
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

void MultiSocketDownloadHandler::handleRequest(RequesterSharedPtr requester)
{
	if (m_randomIntervalRangeTimer->isValid())
	{
		m_requesterQueue.push_back(std::move(requester));
	}
	else
	{
		load(requester);
	}
}

void MultiSocketDownloadHandler::stopRequestHandling(RequesterSharedPtr requester)
{
	return void();
}

QObject* MultiSocketDownloadHandler::qobject()
{
	return this;
}

void MultiSocketDownloadHandler::proxyAuthenticationRequired() const
{
	ServiceLocator* serviceLocator = ServiceLocator::instance();

	ASSERT(serviceLocator->isRegistered<INotificationService>());

	serviceLocator->service<INotificationService>()->error(tr("Proxy error"), tr("Proxy authentication failed"));
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

RequesterSharedPtr MultiSocketDownloadHandler::requesterById(int id) const
{
	const auto requesterIterator = m_requesters.find(id);

	if (requesterIterator == m_requesters.end())
	{
		const int parentId = parentIdFor(id);

		ASSERT(parentId != -1 || !"Invalid request identifier");

		return m_requesters[parentId].lock();
	}

	return requesterIterator.value().lock();
}

int MultiSocketDownloadHandler::parentIdFor(int id) const
{
	const auto redirectBindingIterator = m_idBindings.redirectRequestIdToParentId.find(id);

	if (redirectBindingIterator != m_idBindings.redirectRequestIdToParentId.end())
	{
		return redirectBindingIterator.value();
	}

	return -1;
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

void MultiSocketDownloadHandler::onAboutDownloadProgress(qint64, qint64)
{
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

	const RequesterSharedPtr requester = requesterById(id);

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

int MultiSocketDownloadHandler::maxRedirectsToProcess() const noexcept
{
	return m_maxRedirects;
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

void MultiSocketDownloadHandler::onTimerTicked()
{
	DEBUG_ASSERT(thread() == QThread::currentThread());

	if (m_requesterQueue.empty())
	{
		return;
	}

	RequesterSharedPtr requester(std::move(m_requesterQueue.front()));

	m_requesterQueue.pop_front();

	load(requester);
}

}
