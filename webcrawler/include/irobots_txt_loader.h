#pragma once

namespace WebCrawler
{
	
class IRobotsTxtLoader
{
public:
	virtual ~IRobotsTxtLoader() = default;

	virtual void load(const QUrl& url) = 0;
	virtual const QByteArray& content() const noexcept = 0;
	virtual bool isReady() const noexcept = 0;

	virtual QObject* qobject() = 0;

	// signal
	virtual void ready() = 0;
};

}