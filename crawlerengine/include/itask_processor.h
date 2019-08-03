#pragma once
#include "stdafx.h"

#include "requester.h"

namespace CrawlerEngine
{

class ITaskProcessor
{
public:
	virtual ~ITaskProcessor() = default;

	virtual void handleRequest(RequesterSharedPtr requester) = 0;

	virtual void stopRequestHandling(RequesterSharedPtr requester) = 0;

	virtual QObject* qobject() = 0;
};

}