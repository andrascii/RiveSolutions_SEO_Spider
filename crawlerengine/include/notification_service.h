#pragma once

#include "inotification_service.h"

namespace CrawlerEngine
{

class NotificationService : public QObject, public INotificationService
{
	Q_OBJECT

public:
	virtual void info(const QString& header, const QString& message) override;
	virtual void warning(const QString& header, const QString& message) override;
	virtual void error(const QString& header, const QString& message) override;
	virtual QObject* qobject() override;

signals:
	virtual void addedNotification(NotificationStatus status, const QString& header, const QString& message) override;
};

}