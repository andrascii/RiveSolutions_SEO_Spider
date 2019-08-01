#include "stdafx.h"
#include "license_state_notificator.h"
#include "service_locator.h"
#include "license_state_observer.h"
#include "dialog.h"

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
	const QString popupHeader(tr("Serial number activation"));

	switch (reason)
	{
		case ILicenseStateObserver::ReasonSuccessActivation:
		{
			Dialog::showDeferredMessageBoxDialog(
				popupHeader,
				tr("Your serial number is successfully activated!"),
				QDialogButtonBox::Ok);

			break;
		}
		case ILicenseStateObserver::ReasonDateExpired:
		{
			Dialog::showDeferredMessageBoxDialog(
				popupHeader,
				tr("Your serial number date is over!"),
				QDialogButtonBox::Ok);

			break;
		}
		case ILicenseStateObserver::ReasonRunningTimeOver:
		{
			break;
		}
		case ILicenseStateObserver::ReasonInvalidSerialNumberActivation:
		{
			Dialog::showDeferredMessageBoxDialog(
				popupHeader,
				tr("Unfortunately we detected that your license serial number is invalid"),
				QDialogButtonBox::Ok);

			break;
		}
		case ILicenseStateObserver::ReasonSerialNumberBlacklisted:
		{
			Dialog::showDeferredMessageBoxDialog(
				popupHeader,
				tr("Unfortunately we detected that your license serial number is now blacklisted! "
					"You can notify us about it if you don't understand why this happened."),
				QDialogButtonBox::Ok);

			break;
		}
	}
}

}