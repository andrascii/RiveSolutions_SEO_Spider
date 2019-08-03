#pragma once
#include "stdafx.h"

#include "iworker_page_loader.h"
#include "requester_wrapper.h"

namespace CrawlerEngine
{

class Requester;
class UniqueLinkStore;
struct DownloadResponse;
struct ResetConnectionsResponse;

class MultiRequestPageLoader final
	: public QObject
	, public IWorkerPageLoader
{
	Q_OBJECT

public:
	MultiRequestPageLoader(UniqueLinkStore* uniqueLinkStore, QObject* parent = nullptr);

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
	void removeRequesterAssociatedData(Requester* requester);
	void pauseAllActiveDownloads() const;
	void unpauseAllPausedDownloads() const;
	void resetAllActiveConnections();
	void onResetConnectionsResponse(Requester* requester, const ResetConnectionsResponse&);

private:
	struct RequesterAssociatedData
	{
		RequesterWrapper requesterWrapper;

		//! used when we reloading a page
		//! reloading page storages
		//! the bits where the reloading page is stored
		std::vector<bool> storagesBeforeRemoving;
	};

	QMap<Requester*, RequesterAssociatedData> m_activeRequesters;
	ReceiveState m_state;
	UniqueLinkStore* m_uniqueLinkStore;

	QMap<Requester*, RequesterAssociatedData> m_onAboutClearData;
	RequesterWrapper m_clearWrapper;
};

}
