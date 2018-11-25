#pragma once

#include "abstract_crawler_worker.h"

namespace CrawlerEngine
{

class IHttpClient;

class CrawlerWorkerCurlBased : public AbstractCrawlerWorker
{
	Q_OBJECT

public:
	CrawlerWorkerCurlBased(UniqueLinkStore* uniqueLinkStore);

	virtual std::optional<CrawlerRequest> pendingUrl() const override;

private:
	virtual bool canPullLoading() const override;
	virtual void applyNetworkOptions(const CrawlerOptionsData& optionsData) override;
	virtual void performLoading(const CrawlerRequest& crawlerRequest, DownloadRequest::LinkStatus linkStatus) override;
	virtual void stopLoading() override;
	virtual void clearState() override;
	virtual void addPageReceivedAfterStop(const std::pair<DownloadRequestType, ParsedPagePtr>& pageInfo) override;
	virtual void setPageReceivedAfterStopPromise() override;
	virtual std::vector<WorkerResult> extractLoadedAfterStopPages() override;

private:
	IHttpClient* m_httpClient;
};

}
