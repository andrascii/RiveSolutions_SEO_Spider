#include "notification_service.h"

namespace CrawlerEngine
{

void NotificationService::info(const QString& header, const QString& message)
{
	emit addedNotification(NotificationStatus::NotificationStatusInformation, header, message);
}

void NotificationService::warning(const QString& header, const QString& message)
{
	emit addedNotification(NotificationStatus::NotificationStatusWarning, header, message);
}

void NotificationService::error(const QString& header, const QString& message)
{
	emit addedNotification(NotificationStatus::NotificationStatusError, header, message);
}

QObject* NotificationService::qobject()
{
	return this;
}

}