#pragma once

namespace CrawlerEngine
{

class Url;

class ISpecificLoader
{
public:
	virtual ~ISpecificLoader() = default;

	virtual void setHost(const Url& url) = 0;
	virtual void load() = 0;
	virtual const QByteArray& content() const noexcept = 0;
	virtual bool isReady() const noexcept = 0;
	virtual bool isValid() const noexcept = 0;
	virtual QObject* qobject() = 0;

	// signal
	virtual void ready() = 0;
};

}