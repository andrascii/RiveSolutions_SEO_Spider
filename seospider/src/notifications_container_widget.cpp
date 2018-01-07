#include "notifications_container_widget.h"
#include "notification_popup_widget.h"
#include "service_locator.h"

namespace SeoSpider
{

using namespace CrawlerEngine;

NotificationsContainerWidget::NotificationsContainerWidget(QWidget* parent)
	: QWidget(parent)
	, m_label(new QLabel(this))
	, m_normalPixmap(QStringLiteral(":/images/notifications-normal.png"))
	, m_activePixmap(QStringLiteral(":/images/notifications-active.png"))
	, m_active(false)
	, m_notificationPopup(nullptr)
	, m_currentNotificationIndex(0)
{
	QVBoxLayout* layout = new QVBoxLayout(this);
	layout->addWidget(m_label);

	ServiceLocator* serviceLocator = ServiceLocator::instance();
	INotificationService* notificationService = serviceLocator->service<INotificationService>();

	ASSERT(notificationService);

	VERIFY(connect(notificationService->qobject(), SIGNAL(addedNotification(int, const QString&, const QString&)),
		this, SLOT(onNotificationAdded(int, const QString&, const QString&)), Qt::QueuedConnection));

	m_notifications.emplace_back(NotificationData{ NotificationPopupWidget::Info, tr("Has no notifications"), tr("Has no notifications") });

	changeState();
}

NotificationsContainerWidget::~NotificationsContainerWidget()
{
	if (m_notificationPopup)
	{
		m_notificationPopup->hide();
	}
}

void NotificationsContainerWidget::mouseReleaseEvent(QMouseEvent*)
{
	m_active = !m_active;
	changeState();
}

void NotificationsContainerWidget::onNotificationAdded(int status, const QString& header, const QString& message)
{
	ASSERT(QThread::currentThread() == thread());

	NotificationData notificationData;

	switch (status)
	{
		case INotificationService::NotificationStatusInformation:
		{
			notificationData.status = NotificationPopupWidget::Info;
			break;
		}
		case INotificationService::NotificationStatusWarning:
		{
			notificationData.status = NotificationPopupWidget::Warning;
			break;
		}
		case INotificationService::NotificationStatusError:
		{
			notificationData.status = NotificationPopupWidget::Error;
			break;
		}
		default:
		{
			ASSERT(!"Undefined notification status");
		}
	}

	notificationData.header = header;
	notificationData.message = message;

	m_notifications.emplace_back(std::move(notificationData));
	m_currentNotificationIndex = m_notifications.size() - 1;

	m_active = true;
	changeState();
}

void NotificationsContainerWidget::changeState()
{
	if (m_active)
	{
		if (m_currentNotificationIndex >= m_notifications.size())
		{
			ERRLOG << "Current notification index greater than notifications size";
			m_active = false;
			return;
		}

		const NotificationData& currentNotificationData = m_notifications[m_currentNotificationIndex];

		if (m_notificationPopup)
		{
			m_notificationPopup->close();
		}

		m_notificationPopup = new NotificationPopupWidget(
			static_cast<NotificationPopupWidget::Status>(currentNotificationData.status),
			currentNotificationData.header, 
			currentNotificationData.message, 
			this
		);

		m_notificationPopup->setAttribute(Qt::WA_DeleteOnClose, true);

		m_notificationPopup->show();
		m_label->setPixmap(m_activePixmap);
	}
	else
	{
		if (m_notificationPopup)
		{
			m_notificationPopup->close();
			m_notificationPopup = nullptr;
		}

		m_label->setPixmap(m_normalPixmap);
	}
}

}