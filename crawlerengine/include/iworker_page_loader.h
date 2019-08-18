#pragma once

#include "crawler_request.h"
#include "worker_result.h"
#include "download_request.h"
#include "hops_chain.h"

namespace CrawlerEngine
{

struct CrawlerOptionsData;
struct CrawlerRequest;

class IWorkerPageLoader
{
public:
	enum ReceiveState
	{
		CanReceivePages,
		CantReceivePages
	};

	virtual ~IWorkerPageLoader() = default;

	virtual bool canPullLoading() const = 0;

	virtual void performLoading(const CrawlerRequest& crawlerRequest,
		int turnaround,
		const std::vector<bool>& reloadingPageStrorages,
		DownloadRequest::Status linkStatus) = 0;

	virtual void setReceiveState(ReceiveState state) = 0;

	virtual void clear() = 0;

	virtual QObject* qobject() = 0;

	// signals
	virtual void pageLoaded(const HopsChain& hopsChain,
		int turnaround,
		bool isPageReloaded,
		const std::vector<bool>& reloadingPageStrorages,
		DownloadRequestType requestType) = 0;
};

}
