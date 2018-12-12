#include "dispatcher_based_worker_page_loader.h"
#include "download_response.h"
#include "download_request.h"
#include "crawler_shared_state.h"
#include "pause_connections_request.h"
#include "unpause_connections_request.h"
#include "helpers.h"
#include "unique_link_store.h"

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

DispatcherBasedWorkerPageLoader::DispatcherBasedWorkerPageLoader(UniqueLinkStore* uniqueLinkStore, QObject* parent)
	: QObject(parent)
	, m_state(CanReceivePages)
	, m_uniqueLinkStore(uniqueLinkStore)
{
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

		removeRequesterAssociatedData(requester);
	}
}

bool DispatcherBasedWorkerPageLoader::canPullLoading() const
{
	if (m_state == CantReceivePages)
	{
		return false;
	}

	if (m_uniqueLinkStore->activeUrlCount() > 24)
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
