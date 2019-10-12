#pragma once

#include "inotification_service.h"

namespace SeoSpider
{

using namespace CrawlerEngine;

class NotificationPopupFrame;

class NotificationsContainerWidget : public QWidget
{
	Q_OBJECT

public:
	NotificationsContainerWidget(QWidget* parent);
	virtual ~NotificationsContainerWidget();

protected:
	virtual void mouseReleaseEvent(QMouseEvent* event) override;

private slots:
	void onNotificationAdded(int status, const QString& header, const QString& message);
	void onNotificationFrameDestroyed();

private slots:
	void changeState();

private:
	QLabel* m_label;

	struct NotificationData
	{
		int status;
		QString header;
		QString message;
	};

	QPixmap m_normalPixmap;
	QPixmap m_activePixmap;

	bool m_active;

	QPointer<NotificationPopupFrame> m_notificationPopup;

	std::vector<NotificationData> m_notifications;
	std::size_t m_currentNotificationIndex;

	int m_timeout;
};

}