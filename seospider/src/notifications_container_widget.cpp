#include "notifications_container_widget.h"
#include "notification_popup_frame.h"
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
	, m_timeOut(5000)
{
	QVBoxLayout* layout = new QVBoxLayout(this);
	layout->addWidget(m_label);

	ServiceLocator* serviceLocator = ServiceLocator::instance();
	INotificationService* notificationService = serviceLocator->service<INotificationService>();

	ASSERT(notificationService);

	VERIFY(connect(notificationService->qobject(), SIGNAL(addedNotification(int, const QString&, const QString&)),
		this, SLOT(onNotificationAdded(int, const QString&, const QString&)), Qt::QueuedConnection));

	m_notifications.emplace_back(NotificationData{ NotificationPopupFrame::Info, tr("Has no notifications"), tr("Has no notifications") });

	changeState();
}

NotificationsContainerWidget::~NotificationsContainerWidget()
{
	if (m_notificationPopup)
	{
		disconnect(m_notificationPopup, &NotificationPopupFrame::destroyed,
			this, &NotificationsContainerWidget::onNotificationFrameDestroyed);
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
			notificationData.status = NotificationPopupFrame::Info;
			break;
		}
		case INotificationService::NotificationStatusWarning:
		{
			notificationData.status = NotificationPopupFrame::Warning;
			break;
		}
		case INotificationService::NotificationStatusError:
		{
			notificationData.status = NotificationPopupFrame::Error;
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
	QTimer::singleShot(m_timeOut, this, [&](){m_active = false; changeState();});
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
			disconnect(m_notificationPopup, &NotificationPopupFrame::destroyed,
				this, &NotificationsContainerWidget::onNotificationFrameDestroyed);

			m_notificationPopup->close();
		}

		m_notificationPopup = new NotificationPopupFrame(
			static_cast<NotificationPopupFrame::Status>(currentNotificationData.status),
			currentNotificationData.header, 
			currentNotificationData.message, 
			this
		);

		VERIFY(connect(m_notificationPopup, &NotificationPopupFrame::destroyed, 
			this, &NotificationsContainerWidget::onNotificationFrameDestroyed));

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

void NotificationsContainerWidget::onNotificationFrameDestroyed()
{
	m_active = false;
	changeState();
}

}