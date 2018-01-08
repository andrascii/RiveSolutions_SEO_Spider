#include "host_info_provider.h"
#include "handler_registry.h"
#include "host_info.h"
#include "get_host_info_request.h"
#include "get_host_info_response.h"
#include "thread_message_dispatcher.h"
#include "download_response.h"
#include "crawler_request.h"
#include "download_request.h"

namespace CrawlerEngine
{

HostInfoProvider::HostInfoProvider()
	: QObject(nullptr)
{
#ifdef Q_OS_WIN

	int error = WSAStartup(MAKEWORD(2, 2), &m_wsadata);

	if (error != 0)
	{
		DEBUGLOG << error;

		DEBUG_ASSERT(!"WSAStartup failed with error");
	}

#endif

	HandlerRegistry& handlerRegistry = HandlerRegistry::instance();
	handlerRegistry.registrateHandler(this, RequestType::RequestGetHostInfo);
}

HostInfoProvider::~HostInfoProvider()
{
#ifdef Q_OS_WIN

	WSACleanup();

#endif

	HandlerRegistry& handlerRegistry = HandlerRegistry::instance();
	handlerRegistry.unregistrateHandler(this);
}

void HostInfoProvider::handleRequest(RequesterSharedPtr requester)
{
	ASSERT(requester->request()->requestType() == RequestType::RequestGetHostInfo);
	m_requester = requester;
	GetHostInfoRequest* request = static_cast<GetHostInfoRequest*>(requester->request());
	m_pendingResponse.reset(new GetHostInfoResponse(HostInfo(request->webpage.host().toLatin1())));

	DownloadRequest downloadRequest(CrawlerRequest{ request->webpage, DownloadRequestType::RequestTypeHead });
	m_downloadRequester.reset(downloadRequest, this, &HostInfoProvider::onLoadingDone);
	m_downloadRequester->start();
}

void HostInfoProvider::stopRequestHandling(RequesterSharedPtr requester)
{
	requester;
}

void HostInfoProvider::onLoadingDone(Requester* requester, const DownloadResponse& response)
{
	Q_UNUSED(requester);
	const Hop& hop = response.hopsChain.back();
	
	m_pendingResponse->url = hop.redirectUrl().isValid()
		? hop.redirectUrl() : hop.url();
	
	ThreadMessageDispatcher::forThread(m_requester->thread())->postResponse(m_requester, m_pendingResponse);
	m_pendingResponse.reset();
	m_downloadRequester.reset();
	m_requester.reset();
}

}
