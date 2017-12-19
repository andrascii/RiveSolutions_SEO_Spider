#pragma once

namespace CrawlerEngine
{
	
class INotificationService
{
public:
	virtual ~INotificationService() = default;
	virtual void info(const QString& header, const QString& message) = 0;
	virtual void warning(const QString& header, const QString& message) = 0;
	virtual void error(const QString& header, const QString& message) = 0;
};


}