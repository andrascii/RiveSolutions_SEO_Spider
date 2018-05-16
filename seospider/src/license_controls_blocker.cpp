#include "license_controls_blocker.h"
#include "service_locator.h"
#include "license_service.h"
#include "settings_page_registry.h"

namespace
{

const QVector<QByteArray> s_controlNames
{
	TYPE_STRING(Ui_CompanyProfileSettingsWidget),
	TYPE_STRING(Ui_CrawlerPauseSettingsWidget),
	TYPE_STRING(Ui_LimitsSettingsWidget),
	TYPE_STRING(Ui_PreferencesSettingsWidget),
};

}

namespace SeoSpider
{

using namespace CrawlerEngine;

LicenseControlsBlocker::LicenseControlsBlocker(QObject* parent)
	: QObject(parent)
{
	ISettingsPageRegistry* settingsPageRegistry = ServiceLocator::instance()->service<ISettingsPageRegistry>();

	foreach(auto controlName, s_controlNames)
	{
		addWidget(settingsPageRegistry->settingsPageById(controlName));
	}

	ILicenseService* licenseService = ServiceLocator::instance()->service<ILicenseService>();

	VERIFY(connect(licenseService->qobject(), SIGNAL(licenseChanged(int)), SLOT(onLicenseChanged())));
}

void LicenseControlsBlocker::addWidget(QWidget* object)
{
	ASSERT(object && !m_widgets.contains(object));

	m_widgets.append(object);
}

void LicenseControlsBlocker::onLicenseChanged()
{
	ILicenseService* licenseService = ServiceLocator::instance()->service<ILicenseService>();

	foreach(auto widget, m_widgets)
	{
		widget->setProperty("enabled", !licenseService->isTrialLicense());
	}
}

}
