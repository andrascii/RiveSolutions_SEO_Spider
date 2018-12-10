#pragma once

#include "icrawler_worker_page_loader.h"
#include "requester_wrapper.h"

namespace CrawlerEngine
{

class Requester;
struct DownloadResponse;

class DispatcherBasedWorkerPageLoader final
	: public QObject
	, public ICrawlerWorkerPageLoader
{
	Q_OBJECT

public:
	DispatcherBasedWorkerPageLoader(QObject* parent = nullptr);

	virtual QVector<ResponseData> pendingResponseData() override;

	virtual bool canPullLoading() const override;

	virtual void performLoading(const CrawlerRequest& crawlerRequest,
		const std::vector<bool>& reloadingPageStrorages,
		DownloadRequest::Status linkStatus) override;

	virtual void setReceiveState(ReceiveState state) override;
	virtual void clear() override;
	virtual QObject* qobject() override;

signals:
	virtual void pageLoaded(const HopsChain& hopsChain,
		bool isPageReloaded,
		const std::vector<bool>& reloadingPageStrorages,
		DownloadRequestType requestType) override;

private:
	void onLoadingDone(Requester* requester, const DownloadResponse& response);
	void emitResponseData(const QVector<ResponseData>& responseData);
	void removeRequesterAssociatedData(Requester* requester);
	void reloadPromise();
	void sendRequestToPauseAllActiveDownloads() const;
	void sendRequestToUnpauseAllPausedDownloads() const;

private:
	struct RequesterAssociatedData
	{
		RequesterWrapper requesterWrapper;

		//! used when we reloading a page
		//! reloading page storages
		//! the bits where the reloading page is stored
		std::vector<bool> storagesBeforeRemoving;
	};

	QVector<ResponseData> m_pendingResponseData;
	std::promise<QVector<ResponseData>> m_pendingResponseDataPromise;
	std::future<QVector<ResponseData>> m_pendingResponseDataFuture;

	QMap<Requester*, RequesterAssociatedData> m_activeRequesters;
	ReceiveState m_state;
};

}
