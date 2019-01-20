#include "trial_license_label.h"
#include "license_state_observer.h"
#include "service_locator.h"

namespace SeoSpider
{
TrialLicenseLabel::TrialLicenseLabel(QWidget* parent)
	: QLabel(parent)
{
	setText(QObject::tr("These preferences are not available on the trial version of the application."));
	CrawlerEngine::ILicenseStateObserver* licenseService = CrawlerEngine::ServiceLocator::instance()->service<CrawlerEngine::ILicenseStateObserver>();
	VERIFY(connect(licenseService->qobject(), SIGNAL(licenseChanged(int)), SLOT(onLicenseChanged())));
	onLicenseChanged();
}

void TrialLicenseLabel::onLicenseChanged()
{
	CrawlerEngine::ILicenseStateObserver* licenseService = CrawlerEngine::ServiceLocator::instance()->service<CrawlerEngine::ILicenseStateObserver>();
	const bool isTrialLicense = licenseService->isTrialLicense();
	setVisible(isTrialLicense);
}
}
