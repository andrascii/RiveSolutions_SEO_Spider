#pragma once

namespace CrawlerEngine
{

class CustomUrl;

class IRobotsTxtLoader
{
public:
	virtual ~IRobotsTxtLoader() = default;

	virtual void setUserAgent(const QByteArray& userAgent) = 0;

	virtual void load(const CustomUrl& url) = 0;
	virtual const QByteArray& content() const noexcept = 0;
	virtual bool isReady() const noexcept = 0;

	virtual QObject* qobject() = 0;

	// signal
	virtual void ready() = 0;
};

}