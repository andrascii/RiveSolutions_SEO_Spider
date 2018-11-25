#include "crawler_worker_qt_based_loader.h"
#include "download_response.h"
#include "download_request.h"

namespace CrawlerEngine
{

CrawlerWorkerQtBasedLoader::CrawlerWorkerQtBasedLoader(UniqueLinkStore* uniqueLinkStore)
	: AbstractCrawlerWorker(uniqueLinkStore)
{
}

// this function always must be thread-safe
std::optional<CrawlerRequest> CrawlerWorkerQtBasedLoader::pendingUrl() const
{
	std::optional<CrawlerRequest> value;

	try
	{
		std::future<std::optional<CrawlerRequest>> future = m_pagesAcceptedAfterStop.pagesAcceptedPromise.get_future();
		std::future_status status = future.wait_for(0ms);

		if (status == std::future_status::ready)
		{
			value = future.get();
		}
	}
	catch (const std::future_error& fe)
	{
		WARNLOG << fe.what();
	}

	return value;
}

void CrawlerWorkerQtBasedLoader::onLoadingDone(Requester*, const DownloadResponse& response)
{
	m_downloadRequester.reset();
	AbstractCrawlerWorker::onLoadingDone(response.hopsChain);
}

std::optional<CrawlerRequest> CrawlerWorkerQtBasedLoader::prepareUnloadedPage() const
{
	CrawlerRequest result;

	if (!m_pagesAcceptedAfterStop.pages.empty())
	{
		const auto pair = m_pagesAcceptedAfterStop.pages.front();

		return CrawlerRequest{ pair.second->url, pair.first };
	}

	return std::nullopt;
}

bool CrawlerWorkerQtBasedLoader::canPullLoading() const
{
	return !m_downloadRequester;
}

void CrawlerWorkerQtBasedLoader::applyNetworkOptions(const CrawlerOptionsData&)
{
} // do nothing here

void CrawlerWorkerQtBasedLoader::performLoading(const CrawlerRequest& crawlerRequest, DownloadRequest::LinkStatus linkStatus)
{
	DownloadRequest request(crawlerRequest, linkStatus,
		DownloadRequest::BodyProcessingCommand::CommandAutoDetectionBodyLoadingNecessity, true);

	m_downloadRequester.reset(request, this, &CrawlerWorkerQtBasedLoader::onLoadingDone);
	m_downloadRequester->start();
}

void CrawlerWorkerQtBasedLoader::stopLoading()
{
	if (m_downloadRequester)
	{
		return;
	}

	setPageReceivedAfterStopPromise();
}

void CrawlerWorkerQtBasedLoader::clearState()
{
	m_downloadRequester.reset();
	m_pagesAcceptedAfterStop.pages.clear();
	m_pagesAcceptedAfterStop.pagesAcceptedPromise = std::promise<std::optional<CrawlerRequest>>();
}

void CrawlerWorkerQtBasedLoader::addPageReceivedAfterStop(const std::pair<DownloadRequestType, ParsedPagePtr>& pageInfo)
{
	m_pagesAcceptedAfterStop.pages.push_back(pageInfo);
}

void CrawlerWorkerQtBasedLoader::setPageReceivedAfterStopPromise()
{
	try
	{
		DEBUGLOG << "Set value to promise";

		m_pagesAcceptedAfterStop.pagesAcceptedPromise.set_value(prepareUnloadedPage());
	}
	catch (const std::future_error& error)
	{
		if (error.code() != std::make_error_condition(std::future_errc::promise_already_satisfied))
		{
			throw;
		}
	}
}

std::vector<WorkerResult> CrawlerWorkerQtBasedLoader::extractLoadedAfterStopPages()
{
	std::vector<WorkerResult> loadedAfterStopPages;

	if (!m_pagesAcceptedAfterStop.pages.empty())
	{
		for (const auto& pair : m_pagesAcceptedAfterStop.pages)
		{
			loadedAfterStopPages.emplace_back(pair.second, false, pair.first, std::vector<bool>());
		}
	}

	m_pagesAcceptedAfterStop.pages.clear();
	m_pagesAcceptedAfterStop.pagesAcceptedPromise = std::promise<std::optional<CrawlerRequest>>();

	return loadedAfterStopPages;
}

}
