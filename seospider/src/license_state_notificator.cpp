#include "application.h"
#include "license_state_notificator.h"
#include "service_locator.h"
#include "license_state_observer.h"
#include "dialog.h"
#include "main_window.h"

namespace SeoSpider
{

using namespace CrawlerEngine;

LicenseStateNotificator::LicenseStateNotificator(QObject* parent)
	: QObject(parent)
{
	ILicenseStateObserver* licenseStateObserver = ServiceLocator::instance()->service<ILicenseStateObserver>();
	VERIFY(connect(licenseStateObserver->qobject(), SIGNAL(licenseChanged(int)), this, SLOT(onLicenseStateChanged(int))));
}

void LicenseStateNotificator::onLicenseStateChanged(int reason)
{
	std::function<void()> notification;

	const QString popupHeader(tr("Serial number activation"));

	switch (reason)
	{
		case ILicenseStateObserver::ReasonSuccessActivation:
		{
			notification = [popupHeader]()
			{
				theApp->mainWindow()->showMessageBoxDialog(
					popupHeader,
					tr("Your serial number is successfully activated!"),
					QDialogButtonBox::Ok);
			};

			break;
		}
		case ILicenseStateObserver::ReasonDateExpired:
		{
			notification = [popupHeader]()
			{
				theApp->mainWindow()->showMessageBoxDialog(
					popupHeader,
					tr("Your serial number date is over!"),
					QDialogButtonBox::Ok);
			};

			break;
		}
		case ILicenseStateObserver::ReasonRunningTimeOver:
		{
			break;
		}
		case ILicenseStateObserver::ReasonInvalidSerialNumberActivation:
		{
			notification = [popupHeader]()
			{
				theApp->mainWindow()->showMessageBoxDialog(
					popupHeader,
					tr("Unfortunately we detected that your license serial number is invalid"),
					QDialogButtonBox::Ok);
			};

			break;
		}
		case ILicenseStateObserver::ReasonSerialNumberBlacklisted:
		{
			notification = [popupHeader]()
			{
				theApp->mainWindow()->showMessageBoxDialog(
					popupHeader,
					tr("Unfortunately we detected that your license serial number is now blacklisted! "
						"You can notify us about it if you don't understand why this happened."),
					QDialogButtonBox::Ok);
			};

			break;
		}
	}

	if (Dialog::shownDialog() != nullptr)
	{
		VERIFY(connect(Dialog::shownDialog(), &Dialog::dialogClosed, this, &LicenseStateNotificator::onDialogClosed));
		m_deferredNotifications.push_back(notification);
	}
	else
	{
		notification();
	}
}

void LicenseStateNotificator::onDialogClosed()
{
	for (const std::function<void()>& notification : m_deferredNotifications)
	{
		notification();
	}
}

}