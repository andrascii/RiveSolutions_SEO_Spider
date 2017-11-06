#include "host_info_provider.h"
#include "handler_registry.h"
#include "host_info.h"
#include "get_host_info_request.h"
#include "get_host_info_response.h"
#include "thread_queue.h"

namespace CrawlerEngine
{

HostInfoProvider::HostInfoProvider()
	: QObject(nullptr)
{
	HandlerRegistry& handlerRegistry = HandlerRegistry::instance();
	handlerRegistry.registrateHandler(this, RequestType::RequestGetHostInfo);
}

HostInfoProvider::~HostInfoProvider()
{
	HandlerRegistry& handlerRegistry = HandlerRegistry::instance();
	handlerRegistry.unregistrateHandler(this);
}

void HostInfoProvider::handleRequest(RequesterSharedPtr requester)
{
	ASSERT(requester->request()->requestType() == RequestType::RequestGetHostInfo);
	GetHostInfoRequest* request = static_cast<GetHostInfoRequest*>(requester->request());
	std::shared_ptr<GetHostInfoResponse> response = std::make_shared<GetHostInfoResponse>(HostInfo(request->hostname));

	ThreadQueue::forThread(requester->thread())->postResponse(requester, response);
}

void HostInfoProvider::stopRequestHandling(RequesterSharedPtr requester)
{
	requester;
}

}