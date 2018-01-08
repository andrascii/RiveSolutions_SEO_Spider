#pragma once

#include "requester.h"
#include "requester_wrapper.h"

namespace CrawlerEngine
{
struct DownloadResponse;
struct GetHostInfoResponse;

class HostInfoProvider : public QObject
{
    Q_OBJECT

public:
    HostInfoProvider();
    ~HostInfoProvider();

    Q_INVOKABLE void handleRequest(RequesterSharedPtr requester);
    Q_INVOKABLE void stopRequestHandling(RequesterSharedPtr requester);

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
