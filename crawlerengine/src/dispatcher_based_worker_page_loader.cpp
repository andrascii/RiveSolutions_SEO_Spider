#include "dispatcher_based_worker_page_loader.h"
#include "download_response.h"
#include "download_request.h"

namespace CrawlerEngine
{

DispatcherBasedWorkerPageLoader::DispatcherBasedWorkerPageLoader(QObject* parent)
	: QObject(parent)
{
}

// this function always must be thread-safe
std::optional<CrawlerRequest> DispatcherBasedWorkerPageLoader::pendingUrl() const
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

void DispatcherBasedWorkerPageLoader::onLoadingDone(Requester*, const DownloadResponse& response)
{
	m_downloadRequester.reset();
	emit pageLoaded(response.hopsChain);
}

std::optional<CrawlerRequest> DispatcherBasedWorkerPageLoader::prepareUnloadedPage() const
{
	CrawlerRequest result;

	if (!m_pagesAcceptedAfterStop.pages.empty())
	{
		const auto pair = m_pagesAcceptedAfterStop.pages.front();

		return CrawlerRequest{ pair.second->url, pair.first };
	}

	return std::nullopt;
}

bool DispatcherBasedWorkerPageLoader::canPullLoading() const
{
	return !m_downloadRequester;
}

void DispatcherBasedWorkerPageLoader::applyNetworkOptions(const CrawlerOptionsData&)
{
} // do nothing here

void DispatcherBasedWorkerPageLoader::performLoading(const CrawlerRequest& crawlerRequest, DownloadRequest::LinkStatus linkStatus)
{
	DownloadRequest request(crawlerRequest, linkStatus,
		DownloadRequest::BodyProcessingCommand::CommandAutoDetectionBodyLoading, true);

	m_downloadRequester.reset(request, this, &DispatcherBasedWorkerPageLoader::onLoadingDone);
	m_downloadRequester->start();
}

void DispatcherBasedWorkerPageLoader::stopLoading()
{
	if (m_downloadRequester)
	{
		return;
	}

	setPageReceivedAfterStopPromise();
}

void DispatcherBasedWorkerPageLoader::clearState()
{
	m_downloadRequester.reset();
	m_pagesAcceptedAfterStop.pages.clear();
	m_pagesAcceptedAfterStop.pagesAcceptedPromise = std::promise<std::optional<CrawlerRequest>>();
}

void DispatcherBasedWorkerPageLoader::addPageReceivedAfterStop(const std::pair<DownloadRequestType, ParsedPagePtr>& pageInfo)
{
	m_pagesAcceptedAfterStop.pages.push_back(pageInfo);
}

void DispatcherBasedWorkerPageLoader::setPageReceivedAfterStopPromise()
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

std::vector<WorkerResult> DispatcherBasedWorkerPageLoader::extractLoadedAfterStopPages()
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

QObject* DispatcherBasedWorkerPageLoader::qobject()
{
	return this;
}

}
