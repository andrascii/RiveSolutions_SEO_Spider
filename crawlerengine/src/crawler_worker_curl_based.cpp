#include "crawler_worker_curl_based.h"
#include "http_client.h"

namespace CrawlerEngine
{

CrawlerWorkerCurlBased::CrawlerWorkerCurlBased(UniqueLinkStore* uniqueLinkStore)
	: AbstractCrawlerWorker(uniqueLinkStore)
	, m_httpClient(new HttpClient(this))
{
	qRegisterMetaType<HopsChain>("HopsChain");

	VERIFY(connect(m_httpClient->qobject(), SIGNAL(operationCompleted(const HopsChain&)),
		this, SLOT(onUrlLoaded(const HopsChain&)), Qt::QueuedConnection));
}

std::optional<CrawlerRequest> CrawlerWorkerCurlBased::pendingUrl() const
{
	return std::nullopt;
}

void CrawlerWorkerCurlBased::onUrlLoaded(const HopsChain& hopsChain)
{
	onLoadingDone(hopsChain);
}

bool CrawlerWorkerCurlBased::canPullLoading() const
{
	return true;
}

void CrawlerWorkerCurlBased::applyNetworkOptions(const CrawlerOptionsData& optionsData)
{
	if (optionsData.useProxy)
	{
		m_httpClient->setProxy(
			optionsData.proxyHostName,
			optionsData.proxyPort,
			optionsData.proxyUser,
			optionsData.proxyPassword);
	}

	m_httpClient->setTimeout(optionsData.limitTimeout);
	m_httpClient->setMaxRedirects(optionsData.maxRedirectsToFollow);
	m_httpClient->setUserAgent(optionsData.userAgent);
	if (!optionsData.pauseRangeEnabled)
	{
		m_httpClient->resetPauseRange();
	}
	else
	{
		m_httpClient->setPauseRange(optionsData.pauseRangeFrom, optionsData.pauseRangeTo);
	}
}

void CrawlerWorkerCurlBased::performLoading(const CrawlerRequest& crawlerRequest, DownloadRequest::LinkStatus)
{
	m_httpClient->get(crawlerRequest.url);
}

void CrawlerWorkerCurlBased::stopLoading()
{
} // do nothing

void CrawlerWorkerCurlBased::clearState()
{
} // do nothing

void CrawlerWorkerCurlBased::addPageReceivedAfterStop(const std::pair<DownloadRequestType, ParsedPagePtr>&)
{
} // do nothing

void CrawlerWorkerCurlBased::setPageReceivedAfterStopPromise()
{
} // do nothing

std::vector<WorkerResult> CrawlerWorkerCurlBased::extractLoadedAfterStopPages()
{
	return std::vector<WorkerResult>();
}

}
