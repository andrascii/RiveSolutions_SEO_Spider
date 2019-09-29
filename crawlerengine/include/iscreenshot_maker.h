#pragma once
#ifdef ENABLE_SCREENSHOTS
#include "requester.h"

namespace CrawlerEngine
{

class IScreenshotMaker
{
public:
	virtual ~IScreenshotMaker() = default;

	virtual void handleRequest(RequesterSharedPtr requester) = 0;
	virtual void stopRequestHandling(RequesterSharedPtr requester) = 0;
	virtual QObject* qobject() = 0;
};

}
#endif