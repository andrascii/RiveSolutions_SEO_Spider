#pragma once

#include "requester.h"

namespace CrawlerEngine
{

class IDownloader
{
public:
	virtual ~IDownloader() = default;

	virtual void setPauseRange(int from, int to) = 0;
	virtual void resetPauseRange() = 0;
	virtual void setUserAgent(const QByteArray& userAgent) = 0;
	virtual void handleRequest(RequesterSharedPtr requester) = 0;
	virtual void stopRequestHandling(RequesterSharedPtr requester) = 0;
	virtual QObject* qobject() = 0;
};

}