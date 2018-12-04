#include "dispatcher_based_worker_page_loader.h"
#include "download_response.h"
#include "download_request.h"
#include "crawler_shared_state.h"
#include "helpers.h"

namespace CrawlerEngine
{

DispatcherBasedWorkerPageLoader::DispatcherBasedWorkerPageLoader(QObject* parent)
	: QObject(parent)
	, m_state(CanReceivePages)
{
}

//! this function always must be thread-safe
QVector<ICrawlerWorkerPageLoader::ResponseData>
DispatcherBasedWorkerPageLoader::pendingResponseData()
{
	DEBUG_ASSERT(thread() != QThread::currentThread());

	QVector<ResponseData> responseData;

	try
	{
		std::future<QVector<ResponseData>> future = m_pendingResponseDataPromise.get_future();
		responseData = future.get();
	}
	catch (const std::future_error& futureError)
	{
		WARNLOG << futureError.what();
	}

	return responseData;
}

void DispatcherBasedWorkerPageLoader::onLoadingDone(Requester* requester, const DownloadResponse& response)
{
	m_activeRequesters[requester].requesterWrapper.reset();
	m_activeRequesters.remove(requester);

	const DownloadRequest* downloadRequest =
		Common::Helpers::fast_cast<DownloadRequest*>(requester->request());

	const bool isPageReloaded = downloadRequest->linkStatus == DownloadRequest::Status::LinkStatusReloadAlreadyLoaded;

	if (m_state == CanReceivePages)
	{
		emit pageLoaded(response.hopsChain,
			isPageReloaded,
			m_activeRequesters[requester].storagesBeforeRemoving,
			downloadRequest->requestInfo.requestType);
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

		if (m_activeRequesters.isEmpty())
		{
			m_pendingResponseDataPromise.set_value(std::move(m_pendingResponseData));
			m_pendingResponseData.clear();
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
	constexpr int maxPendingLinksCount = 50;

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
	DEBUG_ASSERT(m_state == CanReceivePages);

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
	m_pendingResponseDataPromise = std::promise<QVector<ResponseData>>();
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

	if (state == CanReceivePages)
	{
		try
		{
			std::future<QVector<ResponseData>> future = m_pendingResponseDataPromise.get_future();
			const std::future_status futureStatus = future.wait_for(0ms);

			if (futureStatus == std::future_status::ready)
			{
				emitResponseData(future.get());
				return;
			}

			emitResponseData(m_pendingResponseData);
			m_pendingResponseData.clear();
		}
		catch (const std::future_error& futureError)
		{
			WARNLOG << futureError.what();
		}
	}
}

void DispatcherBasedWorkerPageLoader::emitResponseData(const QVector<ResponseData>& responseData)
{
	std::for_each(responseData.begin(), responseData.end(), [this](const ResponseData& data)
	{
		emit pageLoaded(data.hopsChain, data.isPageReloaded, data.reloadingPageStrorages, data.requestType);
	});
}

}
