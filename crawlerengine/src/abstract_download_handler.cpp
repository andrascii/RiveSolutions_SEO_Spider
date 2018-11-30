#include "abstract_download_handler.h"
#include "random_interval_range_timer.h"
#include "inotification_service.h"
#include "service_locator.h"
#include "download_request.h"
#include "helpers.h"

namespace CrawlerEngine
{

AbstractDownloadHandler::AbstractDownloadHandler()
	: m_randomIntervalRangeTimer(new RandomIntervalRangeTimer(this))
	, m_maxRedirects(-1)
{
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

void AbstractDownloadHandler::handleRequest(RequesterSharedPtr requester)
{
	DEBUG_ASSERT(thread() == QThread::currentThread());

	ASSERT(requester->request()->requestType() == RequestType::RequestDownload);

#ifndef PRODUCTION

	DownloadRequest* request = Common::Helpers::fast_cast<DownloadRequest*>(requester->request());
	DEBUGLOG << "Loading url:" << request->requestInfo.url.toDisplayString();

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

void AbstractDownloadHandler::removeRequesterFromQueue(RequesterSharedPtr requester)
{
	const auto pendingRequesterIter = std::find(m_requesterQueue.begin(), m_requesterQueue.end(), requester);

	if (pendingRequesterIter != m_requesterQueue.end())
	{
		m_requesterQueue.erase(pendingRequesterIter);
	}
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