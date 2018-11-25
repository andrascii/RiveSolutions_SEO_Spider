#pragma once

#include "hops_chain.h"

namespace CrawlerEngine
{

class IHttpClient
{
public:
	virtual ~IHttpClient() = default;

	virtual void get(const Url& url) = 0;
	virtual void post(const Url& url, const QByteArray& uploadData) = 0;
	virtual void setPauseRange(int from, int to) = 0;
	virtual void resetPauseRange() = 0;
	virtual void setTimeout(int msecs) = 0;
	virtual void setMaxRedirects(int redirects) = 0;
	virtual void setUserAgent(const QByteArray& userAgent) = 0;
	virtual void setProxy(const QString& proxyHostName, int proxyPort, const QString& proxyUser, const QString& proxyPassword) = 0;
	virtual void resetProxy() = 0;

	// signals
	virtual void operationCompleted(const HopsChain& hopsChain) = 0;
};

}
