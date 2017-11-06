#pragma once

#include "requester.h"

namespace CrawlerEngine
{

class IDownloader
{
public:
	virtual ~IDownloader() = default;

	virtual void setUserAgent(const QByteArray& userAgent) = 0;

	virtual void handleRequest(RequesterSharedPtr requester) = 0;

	virtual void stopRequestHandling(RequesterSharedPtr requester) = 0;

	virtual QObject* qobject() = 0;
};

}