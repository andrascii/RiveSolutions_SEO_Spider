#pragma once

#include "iworker_page_loader.h"
#include "requester_wrapper.h"

namespace CrawlerEngine
{

class Requester;
class UniqueLinkStore;
struct DownloadResponse;

class QtPageLoader : public QObject, public IWorkerPageLoader
{
	Q_OBJECT

public:
	QtPageLoader(UniqueLinkStore* uniqueLinkStore);

	virtual bool canPullLoading() const override;

	virtual void performLoading(const CrawlerRequest& crawlerRequest,
		int turnaround,
		const std::vector<bool>& reloadingPageStrorages,
		DownloadRequest::Status linkStatus) override;

	virtual void setReceiveState(ReceiveState state) override;

	virtual void clear() override;

	virtual QObject* qobject() override;

signals:
	virtual void pageLoaded(const HopsChain& hopsChain,
		int turnaround,
		bool isPageReloaded,
		const std::vector<bool>& reloadingPageStrorages,
		DownloadRequestType requestType) override;

private:
	void onLoadingDone(Requester* requester, const DownloadResponse& response);

private:
	ReceiveState m_state;
	UniqueLinkStore* m_uniqueLinkStore;
	std::vector<bool> m_reloadingPageStrorages;
	RequesterWrapper m_requester;
};

}