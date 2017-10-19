#pragma once

namespace WebCrawler
{
	
class IRobotsTxtLoader
{
public:
	virtual ~IRobotsTxtLoader() = default;
	virtual void load(const QUrl& url) = 0;
	virtual void ready(const QByteArray& content) = 0; /* signal */
	virtual QObject* qObject() = 0;
};

}