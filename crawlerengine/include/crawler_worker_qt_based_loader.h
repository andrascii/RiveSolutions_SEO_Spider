#pragma once

#include "abstract_crawler_worker.h"
#include "requester_wrapper.h"

namespace CrawlerEngine
{

class CrawlerWorkerQtBasedLoader : public AbstractCrawlerWorker
{
	Q_OBJECT

public:
	CrawlerWorkerQtBasedLoader(UniqueLinkStore* uniqueLinkStore);

	virtual std::optional<CrawlerRequest> pendingUrl() const override;

private:
	void onLoadingDone(Requester* requester, const DownloadResponse& response);
	std::optional<CrawlerRequest> prepareUnloadedPage() const;

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
	struct PagesAcceptedAfterStop
	{
		using PageRequestPairs = std::vector<std::pair<DownloadRequestType, ParsedPagePtr>>;
		using PagesPromise = std::promise<std::optional<CrawlerRequest>>;

		PageRequestPairs pages;
		PagesPromise mutable pagesAcceptedPromise;
	};

	RequesterWrapper m_downloadRequester;
	PagesAcceptedAfterStop m_pagesAcceptedAfterStop;
};

}
