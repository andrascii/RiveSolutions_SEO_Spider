#pragma once

#include "icrawler_worker_page_loader.h"
#include "requester_wrapper.h"

namespace CrawlerEngine
{

class Requester;
struct DownloadResponse;

class DispatcherBasedWorkerPageLoader final : public QObject, public ICrawlerWorkerPageLoader
{
	Q_OBJECT

public:
	DispatcherBasedWorkerPageLoader(QObject* parent = nullptr);

	virtual std::optional<CrawlerRequest> pendingUrl() const override;
	virtual bool canPullLoading() const override;
	virtual void performLoading(const CrawlerRequest& crawlerRequest, DownloadRequest::LinkStatus linkStatus) override;
	virtual void stopLoading() override;
	virtual void clearState() override;
	virtual void addPageReceivedAfterStop(const std::pair<DownloadRequestType, ParsedPagePtr>& pageInfo) override;
	virtual void setPageReceivedAfterStopPromise() override;
	virtual std::vector<WorkerResult> extractLoadedAfterStopPages() override;

	virtual QObject* qobject() override;

signals:
	virtual void pageLoaded(const HopsChain& hopsChain, int requestType) override;

private:
	void onLoadingDone(Requester* requester, const DownloadResponse& response);
	std::optional<CrawlerRequest> prepareUnloadedPage() const;

private:
	struct PagesAcceptedAfterStop
	{
		using PageRequestPairs = std::vector<std::pair<DownloadRequestType, ParsedPagePtr>>;
		using PagesPromise = std::promise<std::optional<CrawlerRequest>>;

		PageRequestPairs pages;
		PagesPromise mutable pagesAcceptedPromise;
	};

	PagesAcceptedAfterStop m_pagesAcceptedAfterStop;
	QMap<Requester*, RequesterWrapper> m_activeRequesters;
};

}
