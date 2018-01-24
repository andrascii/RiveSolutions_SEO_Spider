#include "tests_host_info_provider.h"
#include "handler_registry.h"
#include "host_info.h"
#include "get_host_info_request.h"
#include "get_host_info_response.h"
#include "thread_message_dispatcher.h"

using namespace CrawlerEngine;

namespace
{

class FakeHostInfo : public CrawlerEngine::HostInfo
{
public:
	FakeHostInfo() = default;
};

}

namespace CrawlerEngineTests
{

TestsHostInfoProvider::TestsHostInfoProvider()
{
	HandlerRegistry& handlerRegistry = HandlerRegistry::instance();
	handlerRegistry.registrateHandler(this, RequestType::RequestGetHostInfo);
}

TestsHostInfoProvider::~TestsHostInfoProvider()
{
	CrawlerEngine::HandlerRegistry& handlerRegistry = HandlerRegistry::instance();
	handlerRegistry.unregistrateHandler(this);
}

void TestsHostInfoProvider::handleRequest(RequesterSharedPtr requester)
{
	ASSERT(requester->request()->requestType() == RequestType::RequestGetHostInfo);

	GetHostInfoRequest* request = static_cast<GetHostInfoRequest*>(requester->request());

	std::shared_ptr<GetHostInfoResponse> response = std::make_shared<GetHostInfoResponse>(FakeHostInfo());
	response->url = request->webpage;

	ThreadMessageDispatcher::forThread(requester->thread())->postResponse(requester, response);
}

void TestsHostInfoProvider::stopRequestHandling(RequesterSharedPtr)
{
}

QObject* TestsHostInfoProvider::qobject()
{
	return this;
}

}