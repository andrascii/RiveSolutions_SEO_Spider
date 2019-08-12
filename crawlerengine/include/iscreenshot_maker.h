#pragma once
#include "stdafx.h"

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