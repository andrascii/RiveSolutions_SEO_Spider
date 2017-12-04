#include "host_info_provider.h"
#include "handler_registry.h"
#include "host_info.h"
#include "get_host_info_request.h"
#include "get_host_info_response.h"
#include "thread_message_dispatcher.h"

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
    GetHostInfoRequest* request = static_cast<GetHostInfoRequest*>(requester->request());
    std::shared_ptr<GetHostInfoResponse> response = std::make_shared<GetHostInfoResponse>(HostInfo(request->hostname));

    ThreadMessageDispatcher::forThread(requester->thread())->postResponse(requester, response);
}

void HostInfoProvider::stopRequestHandling(RequesterSharedPtr requester)
{
    requester;
}

}