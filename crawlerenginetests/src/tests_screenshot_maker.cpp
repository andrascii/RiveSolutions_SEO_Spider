#ifdef ENABLE_SCREENSHOTS
#include "tests_screenshot_maker.h"
#include "thread_message_dispatcher.h"
#include "take_screenshot_response.h"
#include "handler_registry.h"

using namespace CrawlerEngine;

namespace CrawlerEngineTests
{

TestsScreenshotMaker::TestsScreenshotMaker(QObject* parent)
	: QObject(parent)
{
	HandlerRegistry& handlerRegistry = HandlerRegistry::instance();
	handlerRegistry.registrateHandler(this, RequestType::RequestTakeScreenshot);
}

void TestsScreenshotMaker::handleRequest(RequesterSharedPtr requester)
{
	ThreadMessageDispatcher::forThread(requester->thread())->postResponse(requester, std::make_shared<TakeScreenshotResponseTest>());
}

void TestsScreenshotMaker::stopRequestHandling(RequesterSharedPtr requester)
{
}

QObject* TestsScreenshotMaker::qobject()
{
	return this;
}

}
#endif
