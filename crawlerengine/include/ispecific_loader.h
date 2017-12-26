#pragma once

namespace CrawlerEngine
{

class CustomUrl;

class ISpecificLoader
{
public:
	virtual ~ISpecificLoader() = default;

	virtual void load(const CustomUrl& url) = 0;
	virtual const QByteArray& content() const noexcept = 0;
	virtual bool isReady() const noexcept = 0;
	virtual bool isValid() const noexcept = 0;
	virtual QObject* qobject() = 0;

	// signal
	virtual void ready() = 0;
};

}