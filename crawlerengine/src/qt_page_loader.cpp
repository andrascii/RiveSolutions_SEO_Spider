#include "qt_page_loader.h"
#include "download_response.h"
#include "helpers.h"
#include "crawler_shared_state.h"

namespace CrawlerEngine
{

QtPageLoader::QtPageLoader(UniqueLinkStore* uniqueLinkStore)
	: m_state(CanReceivePages)
	, m_uniqueLinkStore(uniqueLinkStore)
{
}

bool QtPageLoader::canPullLoading() const
{
	if (m_requester.get() != nullptr || m_state == CantReceivePages)
	{
		return false;
	}

	constexpr int maxAdditionalPendingCount = 150;
	if (CrawlerSharedState::instance()->additionalPendingCount() > maxAdditionalPendingCount)
	{
		return false;
	}

	return true;
}

void QtPageLoader::performLoading(const CrawlerRequest& crawlerRequest,
	int turnaround,
	const std::vector<bool>& reloadingPageStrorages,
	DownloadRequest::Status linkStatus)
{
	DEBUG_ASSERT(m_state == CanReceivePages || linkStatus == DownloadRequest::Status::LinkStatusReloadAlreadyLoaded);

	m_reloadingPageStrorages = reloadingPageStrorages;

	DownloadRequest request(crawlerRequest, turnaround, linkStatus,
		DownloadRequest::BodyProcessingCommand::CommandAutoDetectionBodyLoading, true);

	m_requester.reset(request, this, &QtPageLoader::onLoadingDone);
	m_requester->start();
}

void QtPageLoader::setReceiveState(ReceiveState state)
{
	DEBUG_ASSERT(thread() == QThread::currentThread());
	ASSERT(state == CanReceivePages || state == CantReceivePages);

	m_state = state;
}

void QtPageLoader::clear()
{
	m_requester.reset();
}

QObject* QtPageLoader::qobject()
{
	return this;
}

void QtPageLoader::onLoadingDone(Requester* requester, const DownloadResponse& response)
{
	const DownloadRequest* downloadRequest =
		Common::Helpers::fast_cast<DownloadRequest*>(requester->request());

	const bool isPageReloaded = downloadRequest->linkStatus == DownloadRequest::Status::LinkStatusReloadAlreadyLoaded;

	emit pageLoaded(response.hopsChain,
		downloadRequest->turnaround,
		isPageReloaded,
		m_reloadingPageStrorages,
		downloadRequest->requestInfo.requestType);

	m_requester.reset();
}

}
