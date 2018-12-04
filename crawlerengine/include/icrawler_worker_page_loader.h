#pragma once

#include "crawler_request.h"
#include "worker_result.h"
#include "download_request.h"
#include "hops_chain.h"

namespace CrawlerEngine
{

struct CrawlerOptionsData;
struct CrawlerRequest;

class ICrawlerWorkerPageLoader
{
public:
	enum ReceiveState
	{
		CanReceivePages,
		CantReceivePages
	};

	struct ResponseData
	{
		HopsChain hopsChain;
		std::vector<bool> reloadingPageStrorages;
		DownloadRequestType requestType;
		bool isPageReloaded;
	};

	virtual ~ICrawlerWorkerPageLoader() = default;

	virtual QVector<ResponseData> pendingResponseData() = 0;

	virtual bool canPullLoading() const = 0;

	virtual void performLoading(const CrawlerRequest& crawlerRequest,
		const std::vector<bool>& reloadingPageStrorages,
		DownloadRequest::Status linkStatus) = 0;

	virtual void setReceiveState(ReceiveState state) = 0;

	virtual void clear() = 0;

	virtual QObject* qobject() = 0;

	// signals
	virtual void pageLoaded(const HopsChain& hopsChain,
		bool isPageReloaded,
		const std::vector<bool>& reloadingPageStrorages,
		DownloadRequestType requestType) = 0;
};

}
