#pragma once

#include "crawler_request.h"
#include "worker_result.h"
#include "download_request.h"

namespace CrawlerEngine
{

class HopsChain;
struct CrawlerOptionsData;
struct CrawlerRequest;

class ICrawlerWorkerPageLoader
{
public:
	virtual ~ICrawlerWorkerPageLoader() = default;
	virtual std::optional<CrawlerRequest> pendingUrl() const = 0;
	virtual bool canPullLoading() const = 0;
	virtual void applyNetworkOptions(const CrawlerOptionsData& optionsData) = 0;
	virtual void performLoading(const CrawlerRequest& crawlerRequest, DownloadRequest::LinkStatus linkStatus) = 0;
	virtual void stopLoading() = 0;
	virtual void clearState() = 0;
	virtual void addPageReceivedAfterStop(const std::pair<DownloadRequestType, ParsedPagePtr>& pageInfo) = 0;
	virtual void setPageReceivedAfterStopPromise() = 0;
	virtual std::vector<WorkerResult> extractLoadedAfterStopPages() = 0;

	virtual QObject* qobject() = 0;

	// signals
	virtual void pageLoaded(const HopsChain& hopsChain) = 0;
};

}
