#pragma once

namespace CrawlerEngine
{

class INotificationService
{
public:
	enum NotificationStatus
	{
		NotificationStatusInformation,
		NotificationStatusWarning,
		NotificationStatusError
	};

	virtual ~INotificationService() = default;
	virtual void info(const QString& header, const QString& message) = 0;
	virtual void warning(const QString& header, const QString& message) = 0;
	virtual void error(const QString& header, const QString& message) = 0;
	virtual QObject* qobject() = 0;

	// signal
	virtual void addedNotification(NotificationStatus status, const QString& header, const QString& message) = 0;
};

}