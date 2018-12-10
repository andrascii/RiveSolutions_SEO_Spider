#include "dispatcher_based_worker_page_loader.h"
#include "download_response.h"
#include "download_request.h"
#include "crawler_shared_state.h"
#include "pause_connections_request.h"
#include "unpause_connections_request.h"
#include "helpers.h"

namespace
{

using namespace CrawlerEngine;

QString printReceiveState(ICrawlerWorkerPageLoader::ReceiveState state)
{
	DEBUG_ASSERT(state == ICrawlerWorkerPageLoader::CanReceivePages ||
		state == ICrawlerWorkerPageLoader::CantReceivePages);

	switch (state)
	{
		case ICrawlerWorkerPageLoader::CanReceivePages: return QStringLiteral("CanReceivePages State");
		case ICrawlerWorkerPageLoader::CantReceivePages: return QStringLiteral("CantReceivePages State");
	}

	return QString();
}

}

namespace CrawlerEngine
{

DispatcherBasedWorkerPageLoader::DispatcherBasedWorkerPageLoader(QObject* parent)
	: QObject(parent)
	, m_state(CanReceivePages)
{
	reloadPromise();
}

//! this function always must be thread-safe
QVector<ICrawlerWorkerPageLoader::ResponseData>
DispatcherBasedWorkerPageLoader::pendingResponseData()
{
	DEBUG_ASSERT(thread() != QThread::currentThread());

	QVector<ResponseData> responseData;

	try
	{
		responseData = m_pendingResponseDataFuture.get();
	}
	catch (const std::future_error& futureError)
	{
		WARNLOG << futureError.what();
	}

	return responseData;
}

void DispatcherBasedWorkerPageLoader::onLoadingDone(Requester* requester, const DownloadResponse& response)
{
	DEBUG_ASSERT(m_activeRequesters.contains(requester));

	const DownloadRequest* downloadRequest =
		Common::Helpers::fast_cast<DownloadRequest*>(requester->request());

	const bool isPageReloaded = downloadRequest->linkStatus == DownloadRequest::Status::LinkStatusReloadAlreadyLoaded;

	if (m_state == CanReceivePages || isPageReloaded)
	{
		emit pageLoaded(response.hopsChain,
			isPageReloaded,
			m_activeRequesters[requester].storagesBeforeRemoving,
			downloadRequest->requestInfo.requestType);

		removeRequesterAssociatedData(requester);
	}
	else
	{
		ResponseData responseData
		{
			std::move(response.hopsChain),
			m_activeRequesters[requester].storagesBeforeRemoving,
			downloadRequest->requestInfo.requestType,
			isPageReloaded
		};

		m_pendingResponseData.insert(m_pendingResponseData.end(), std::move(responseData));

		removeRequesterAssociatedData(requester);

		if (m_activeRequesters.isEmpty())
		{
			INFOLOG << "Set promise";
			m_pendingResponseDataPromise.set_value(std::move(m_pendingResponseData));
		}
	}
}

bool DispatcherBasedWorkerPageLoader::canPullLoading() const
{
	if (m_state == CantReceivePages)
	{
		return false;
	}

	const CrawlerSharedState* state = CrawlerSharedState::instance();
	const int workersProcessedLinksCount = state->workersProcessedLinksCount();
	const int downloaderCrawledLinksCount = state->downloaderCrawledLinksCount();

	const int differenceBetweenWorkersAndDownloader = downloaderCrawledLinksCount - workersProcessedLinksCount;
	constexpr int maxPendingLinksCount = 16;

	if (differenceBetweenWorkersAndDownloader > maxPendingLinksCount)
	{
		return false;
	}

	return true;
}

void DispatcherBasedWorkerPageLoader::performLoading(const CrawlerRequest& crawlerRequest,
	const std::vector<bool>& reloadingPageStrorages,
	DownloadRequest::Status linkStatus)
{
	DEBUG_ASSERT(m_state == CanReceivePages || linkStatus == DownloadRequest::Status::LinkStatusReloadAlreadyLoaded);

	DownloadRequest request(crawlerRequest, linkStatus,
		DownloadRequest::BodyProcessingCommand::CommandAutoDetectionBodyLoading, true);

	RequesterWrapper requesterWrapper;

	requesterWrapper.reset(request, this, &DispatcherBasedWorkerPageLoader::onLoadingDone);
	requesterWrapper->start();

	m_activeRequesters[requesterWrapper.get()] = RequesterAssociatedData{ requesterWrapper, reloadingPageStrorages };
}

void DispatcherBasedWorkerPageLoader::clear()
{
	m_activeRequesters.clear();
	m_pendingResponseData.clear();

	reloadPromise();
}

QObject* DispatcherBasedWorkerPageLoader::qobject()
{
	return this;
}

void DispatcherBasedWorkerPageLoader::setReceiveState(ReceiveState state)
{
	DEBUG_ASSERT(thread() == QThread::currentThread());
	ASSERT(state == CanReceivePages || state == CantReceivePages);

	m_state = state;

	INFOLOG << printReceiveState(m_state);

	if (state == CanReceivePages)
	{
		try
		{
			const std::future_status futureStatus = m_pendingResponseDataFuture.wait_for(0ms);

			if (futureStatus == std::future_status::ready)
			{
				emitResponseData(m_pendingResponseDataFuture.get());
				reloadPromise();
				return;
			}

			emitResponseData(m_pendingResponseData);
			m_pendingResponseData.clear();
		}
		catch (const std::future_error& futureError)
		{
			WARNLOG << futureError.what();
		}

		sendRequestToUnpauseAllPausedDownloads();
	}
	else
	{
		sendRequestToPauseAllActiveDownloads();
	}
}

void DispatcherBasedWorkerPageLoader::emitResponseData(const QVector<ResponseData>& responseData)
{
	std::for_each(responseData.begin(), responseData.end(), [this](const ResponseData& data)
	{
		emit pageLoaded(data.hopsChain, data.isPageReloaded, data.reloadingPageStrorages, data.requestType);
	});
}

void DispatcherBasedWorkerPageLoader::removeRequesterAssociatedData(Requester* requester)
{
	m_activeRequesters[requester].requesterWrapper.reset();
	m_activeRequesters.remove(requester);
}

void DispatcherBasedWorkerPageLoader::reloadPromise()
{
	m_pendingResponseDataPromise = std::promise<QVector<ResponseData>>();
	m_pendingResponseDataFuture = m_pendingResponseDataPromise.get_future();
}

void DispatcherBasedWorkerPageLoader::sendRequestToPauseAllActiveDownloads() const
{
	PauseConnectionsRequest pauseRequest(m_activeRequesters.keys());
	RequesterWrapper requester;
	requester.reset(pauseRequest);
	requester->start();
}

void DispatcherBasedWorkerPageLoader::sendRequestToUnpauseAllPausedDownloads() const
{
	UnpauseConnectionsRequest pauseRequest(m_activeRequesters.keys());
	RequesterWrapper requester;
	requester.reset(pauseRequest);
	requester->start();
}

}
