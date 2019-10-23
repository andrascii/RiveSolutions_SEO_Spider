#include "stdafx.h"
#include "abstract_download_handler.h"
#include "random_interval_range_timer.h"
#include "inotification_service.h"
#include "service_locator.h"
#include "download_request.h"
#include "helpers.h"
#include "handler_registry.h"
#include "pause_connections_request.h"
#include "unpause_connections_request.h"
#include "reset_connections_request.h"
#include "reset_connections_response.h"
#include "thread_message_dispatcher.h"

namespace
{

constexpr int c_maxParallelTransferCount = 50;
constexpr int c_defaultParallelTransferCount = 24;

}

namespace CrawlerEngine
{

AbstractDownloadHandler::AbstractDownloadHandler()
	: m_randomIntervalRangeTimer(new RandomIntervalRangeTimer(this))
	, m_maxRedirects(-1)
	, m_maxParallelConnections(c_defaultParallelTransferCount)
{
	HandlerRegistry& handlerRegistry = HandlerRegistry::instance();
	handlerRegistry.registrateHandler(this, RequestType::RequestDownload);
	handlerRegistry.registrateHandler(this, RequestType::RequestPauseConnections);
	handlerRegistry.registrateHandler(this, RequestType::RequestUnpauseConnections);
	handlerRegistry.registrateHandler(this, RequestType::RequestResetConnections);

	VERIFY(connect(m_randomIntervalRangeTimer, &RandomIntervalRangeTimer::timerTicked,
		this, &AbstractDownloadHandler::onTimerTicked, Qt::DirectConnection));
}

void AbstractDownloadHandler::setPauseRange(int from, int to)
{
	resetPauseRange();
	m_randomIntervalRangeTimer->setRange(from, to);
	m_randomIntervalRangeTimer->start();
}

void AbstractDownloadHandler::resetPauseRange()
{
	DEBUG_ASSERT(thread() == QThread::currentThread() && "This method should be called from the same thread");
	m_randomIntervalRangeTimer->reset();
}

void AbstractDownloadHandler::setMaxRedirects(int redirects)
{
	m_maxRedirects = redirects;
}

void AbstractDownloadHandler::setMaxParallelConnections(int connections)
{
	m_maxParallelConnections = connections;
}

void AbstractDownloadHandler::handleRequest(RequesterSharedPtr requester)
{
	DEBUG_ASSERT(thread() == QThread::currentThread());

	const RequestType requestType = requester->request()->requestType();

	ASSERT(requestType == RequestType::RequestDownload ||
		requestType == RequestType::RequestPauseConnections ||
		requestType == RequestType::RequestUnpauseConnections ||
		requestType == RequestType::RequestResetConnections);

	if (requestType == RequestType::RequestPauseConnections)
	{
		PauseConnectionsRequest* request = Common::Helpers::fast_cast<PauseConnectionsRequest*>(requester->request());
		pauseRequesters(request->requestersToPause);
		return;
	}
	if (requestType == RequestType::RequestUnpauseConnections)
	{
		UnpauseConnectionsRequest* request = Common::Helpers::fast_cast<UnpauseConnectionsRequest*>(requester->request());
		unpauseRequesters(request->requestersToUnpause);
		return;
	}
	if (requestType == RequestType::RequestResetConnections)
	{
		ResetConnectionsRequest* request = Common::Helpers::fast_cast<ResetConnectionsRequest*>(requester->request());
		resetRequesters(request->requestersToReset);
		ThreadMessageDispatcher::forThread(requester->thread())->postResponse(requester, std::make_shared<ResetConnectionsResponse>());
		return;
	}

#ifndef PRODUCTION

	DownloadRequest* request = Common::Helpers::fast_cast<DownloadRequest*>(requester->request());
	DEBUGLOG << "Loading url: " << request->requestInfo.url.toDisplayString();

#endif

	if (m_randomIntervalRangeTimer->isValid())
	{
		m_requesterQueue.push_back(std::move(requester));
	}
	else
	{
		load(requester);
	}
}

int AbstractDownloadHandler::maxRedirectsToProcess() const noexcept
{
	constexpr int maxRedirectsToProcessAnyway = std::numeric_limits<int>::max();

	if (m_maxRedirects > 0)
	{
		return qMin(m_maxRedirects, maxRedirectsToProcessAnyway);
	}

	return maxRedirectsToProcessAnyway;
}

int AbstractDownloadHandler::maxParrallelConnections() const noexcept
{
	return qBound(1, m_maxParallelConnections, c_maxParallelTransferCount);
}

void AbstractDownloadHandler::removeRequesterFromQueue(RequesterSharedPtr requester)
{
	const auto pendingRequesterIter = std::find(m_requesterQueue.begin(), m_requesterQueue.end(), requester);

	if (pendingRequesterIter != m_requesterQueue.end())
	{
		m_requesterQueue.erase(pendingRequesterIter);
	}
}

bool AbstractDownloadHandler::isRedirectionStatusCodeAndResponseHasLocation(Common::StatusCode statusCode) const
{
	return statusCode == Common::StatusCode::MovedPermanently301 ||
		statusCode == Common::StatusCode::MovedTemporarily302 ||
		statusCode == Common::StatusCode::TemporaryRedirect307 ||
		statusCode == Common::StatusCode::PermanentRedirect308;
}

void AbstractDownloadHandler::proxyAuthenticationRequired() const
{
	ServiceLocator* serviceLocator = ServiceLocator::instance();

	ASSERT(serviceLocator->isRegistered<INotificationService>());

	serviceLocator->service<INotificationService>()->error(tr("Proxy error"), tr("Proxy authentication failed"));
}

void AbstractDownloadHandler::onTimerTicked()
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