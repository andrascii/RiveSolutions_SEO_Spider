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
	Common::HandlerRegistry& handlerRegistry = Common::HandlerRegistry::instance();
	handlerRegistry.registrateHandler(this, Common::RequestType::RequestGetHostInfo);
}

void HostInfoProvider::handleRequest(Common::RequesterSharedPtr requester)
{
	ASSERT(requester->request()->requestType() == Common::RequestType::RequestGetHostInfo);
	GetHostInfoRequest* request = static_cast<GetHostInfoRequest*>(requester->request());
	std::shared_ptr<GetHostInfoResponse> response = std::make_shared<GetHostInfoResponse>(HostInfo(request->hostname));

	Common::ThreadQueue::forThread(requester->thread())->postResponse(requester, response);
}

void HostInfoProvider::stopRequestHandling(Common::RequesterSharedPtr requester)
{
	requester;
}

}