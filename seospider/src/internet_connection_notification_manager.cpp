#include "internet_connection_notification_manager.h"
#include "internet_connection_inspector.h"

namespace SeoSpider
{

InternetConnectionNotificationManager::InternetConnectionNotificationManager(QObject* parent)
	: QObject(parent)
	, m_internetAvailable(false)
{
	DEBUG_ASSERT(!s_selfPointer);

	s_selfPointer = this;

	InternetConnectionInspector::init();

	VERIFY(connect(InternetConnectionInspector::instance(), SIGNAL(statusChanged(bool)), 
		this, SLOT(onAboutInternetConnectionStateChanged(bool)), Qt::QueuedConnection));
}

InternetConnectionNotificationManager::~InternetConnectionNotificationManager()
{
	InternetConnectionInspector::term();
}

bool InternetConnectionNotificationManager::internetAvailable() const
{
	return m_internetAvailable;
}

void InternetConnectionNotificationManager::onAboutInternetConnectionStateChanged(bool value)
{
	m_internetAvailable = value;

	emit internetConnectionStateChanged();
}

InternetConnectionNotificationManager* InternetConnectionNotificationManager::s_selfPointer = nullptr;

}