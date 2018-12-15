#include "multi_request_page_loader.h"
#include "reset_connections_request.h"
#include "reset_connections_response.h"
#include "download_response.h"
#include "download_request.h"
#include "crawler_shared_state.h"
#include "pause_connections_request.h"
#include "unpause_connections_request.h"
#include "unique_link_store.h"
#include "helpers.h"

namespace
{

using namespace CrawlerEngine;

QString printReceiveState(IWorkerPageLoader::ReceiveState state)
{
	DEBUG_ASSERT(state == IWorkerPageLoader::CanReceivePages ||
		state == IWorkerPageLoader::CantReceivePages);

	switch (state)
	{
		case IWorkerPageLoader::CanReceivePages: return QStringLiteral("CanReceivePages State");
		case IWorkerPageLoader::CantReceivePages: return QStringLiteral("CantReceivePages State");
	}

	return QString();
}

}

namespace CrawlerEngine
{

MultiRequestPageLoader::MultiRequestPageLoader(UniqueLinkStore* uniqueLinkStore, QObject* parent)
	: QObject(parent)
	, m_state(CanReceivePages)
	, m_uniqueLinkStore(uniqueLinkStore)
{
}

void MultiRequestPageLoader::onLoadingDone(Requester* requester, const DownloadResponse& response)
{
	DEBUG_ASSERT(m_activeRequesters.contains(requester) || m_onAboutClearData.contains(requester));

	const DownloadRequest* downloadRequest =
		Common::Helpers::fast_cast<DownloadRequest*>(requester->request());

	const bool isPageReloaded = downloadRequest->linkStatus == DownloadRequest::Status::LinkStatusReloadAlreadyLoaded;

	emit pageLoaded(response.hopsChain,
		isPageReloaded,
		m_activeRequesters[requester].storagesBeforeRemoving,
		downloadRequest->requestInfo.requestType);

	removeRequesterAssociatedData(requester);
}

bool MultiRequestPageLoader::canPullLoading() const
{
	if (m_state == CantReceivePages)
	{
		return false;
	}

	if (m_uniqueLinkStore->activeUrlCount() > 50)
	{
		return false;
	}

	return true;
}

void MultiRequestPageLoader::performLoading(const CrawlerRequest& crawlerRequest,
	const std::vector<bool>& reloadingPageStrorages,
	DownloadRequest::Status linkStatus)
{
	DEBUG_ASSERT(m_state == CanReceivePages || linkStatus == DownloadRequest::Status::LinkStatusReloadAlreadyLoaded);

	DownloadRequest request(crawlerRequest, linkStatus,
		DownloadRequest::BodyProcessingCommand::CommandAutoDetectionBodyLoading, true);

	RequesterWrapper requesterWrapper;

	requesterWrapper.reset(request, this, &MultiRequestPageLoader::onLoadingDone);
	requesterWrapper->start();

	m_activeRequesters[requesterWrapper.get()] = RequesterAssociatedData{ requesterWrapper, reloadingPageStrorages };
}

void MultiRequestPageLoader::clear()
{
	resetAllActiveConnections();

	// in case if user clears the data and after that immediately stars the new crawling
	// we copying all active requesters in order to delete all them after the response of unpause operation is received
	// if we will not to copy container then we can clear only m_activeRequesters which can store at that moment requesters
	// belongs to the new crawling session
	m_onAboutClearData = m_activeRequesters;
	m_activeRequesters.clear();
}

QObject* MultiRequestPageLoader::qobject()
{
	return this;
}

void MultiRequestPageLoader::setReceiveState(ReceiveState state)
{
	DEBUG_ASSERT(thread() == QThread::currentThread());
	ASSERT(state == CanReceivePages || state == CantReceivePages);

	m_state = state;

	INFOLOG << printReceiveState(m_state);

	if (state == CanReceivePages)
	{
		unpauseAllPausedDownloads();
	}
	else
	{
		pauseAllActiveDownloads();
	}
}

void MultiRequestPageLoader::emitResponseData(const QVector<ResponseData>& responseData)
{
	std::for_each(responseData.begin(), responseData.end(), [this](const ResponseData& data)
	{
		emit pageLoaded(data.hopsChain, data.isPageReloaded, data.reloadingPageStrorages, data.requestType);
	});
}

void MultiRequestPageLoader::removeRequesterAssociatedData(Requester* requester)
{
	m_activeRequesters[requester].requesterWrapper.reset();
	m_activeRequesters.remove(requester);
}

void MultiRequestPageLoader::pauseAllActiveDownloads() const
{
	PauseConnectionsRequest pauseRequest(m_activeRequesters.keys());
	RequesterWrapper requester;
	requester.reset(pauseRequest);
	requester->start();
}

void MultiRequestPageLoader::unpauseAllPausedDownloads() const
{
	UnpauseConnectionsRequest unpauseRequest(m_activeRequesters.keys());
	RequesterWrapper requester;
	requester.reset(unpauseRequest);
	requester->start();
}

void MultiRequestPageLoader::onResetConnectionsResponse(Requester* requester, const ResetConnectionsResponse&)
{
	requester->deleteLater();
	m_onAboutClearData.clear();
	m_clearWrapper.reset();
}

void MultiRequestPageLoader::resetAllActiveConnections()
{
	ResetConnectionsRequest unpauseRequest(m_activeRequesters.keys());
	m_clearWrapper.reset(unpauseRequest, this, &MultiRequestPageLoader::onResetConnectionsResponse);
	m_clearWrapper->start();
}

}
