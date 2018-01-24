#pragma once

#include "requester.h"
#include "requester_wrapper.h"
#include "ihost_info_provider.h"

namespace CrawlerEngine
{
struct DownloadResponse;
struct GetHostInfoResponse;

class HostInfoProvider : public QObject, public IHostInfoProvider
{
	Q_OBJECT

public:
	HostInfoProvider();
	~HostInfoProvider();

	Q_INVOKABLE virtual void handleRequest(RequesterSharedPtr requester) override;
	Q_INVOKABLE virtual void stopRequestHandling(RequesterSharedPtr requester) override;
	virtual QObject* qobject() override;

private:
	void onLoadingDone(Requester* requester, const DownloadResponse& response);

#ifdef Q_OS_WIN

private:
	WSADATA m_wsadata;

#endif

private:
	RequesterSharedPtr m_requester;
	RequesterWrapper m_downloadRequester;
	std::shared_ptr<GetHostInfoResponse> m_pendingResponse;
};

}
