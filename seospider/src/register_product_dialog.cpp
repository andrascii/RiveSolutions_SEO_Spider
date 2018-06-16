#include "register_product_dialog.h"
#include "service_locator.h"
#include "license_service.h"
#include "set_serial_number_request.h"
#include "set_serial_number_response.h"
#include "license_state.h"

namespace SeoSpider
{

using namespace CrawlerEngine;

RegisterProductDialog::RegisterProductDialog(QWidget* parent)
	: Dialog(parent)
	, m_ui(new Ui_RegisterProductDialog)
{
	m_ui->setupUi(this);

	setWindowFlags(Qt::Dialog);

	Qt::WindowFlags flags = windowFlags();
	Qt::WindowFlags helpFlag = Qt::WindowContextHelpButtonHint;
	flags = flags & (~helpFlag);
	setWindowFlags(flags);

	setWindowModality(Qt::ApplicationModal);

	ILicenseService* licenseService = ServiceLocator::instance()->service<ILicenseService>();
	VERIFY(connect(licenseService->qobject(), SIGNAL(licenseChanged(int)), SLOT(onLicenseChanged())));
	VERIFY(connect(m_ui->enterKeyPushButton, &QPushButton::clicked, this, &RegisterProductDialog::onEnterSerialNumber));

	onLicenseChanged();
}

void RegisterProductDialog::onLicenseChanged()
{
	ILicenseService* licenseService = ServiceLocator::instance()->service<ILicenseService>();
	const bool isTrialLicense = licenseService->isTrialLicense();

	m_ui->licenseInfoLabel->setText(isTrialLicense ?
		tr("License invalid.") :
		tr("License valid.")
	);

	m_ui->licenseKeyTextEdit->setEnabled(isTrialLicense);
	m_ui->enterKeyPushButton->setEnabled(isTrialLicense);
}

void RegisterProductDialog::onEnterSerialNumber()
{
	if (m_licenseActivationRequester)
	{
		return;
	}

	const QByteArray serialNumber = m_ui->licenseKeyTextEdit->toPlainText().toLatin1();

	m_licenseActivationRequester.reset(CrawlerEngine::SetSerialNumberRequest(serialNumber), this, &RegisterProductDialog::onActivationSerialNumberResult);
	m_licenseActivationRequester->start();
}

void RegisterProductDialog::onActivationSerialNumberResult(CrawlerEngine::Requester*, const CrawlerEngine::SetSerialNumberResponse& response)
{
	m_licenseActivationRequester.reset();

	LicenseStateFlags stateFlags = response.state();

	if (stateFlags.testFlag(SERIAL_STATE_FLAG_INVALID))
	{
		m_ui->licenseInfoLabel->setText(tr("<font color='red'>The entered serial number is invalid! Try again maybe you're lucky.</font>"));
		return;
	}

	if (stateFlags.testFlag(SERIAL_STATE_FLAG_BLACKLISTED))
	{
		m_ui->licenseInfoLabel->setText(tr("<font color='red'>Oops. This serial number is in the black list!</font>"));
		return;
	}

	if (stateFlags.testFlag(SERIAL_STATE_FLAG_MAX_BUILD_EXPIRED))
	{
		m_ui->licenseInfoLabel->setText(tr("<font color='red'>This serial number activation period is expired! Bad news.</font>"));
		return;
	}

	if (stateFlags.testFlag(SERIAL_STATE_FLAG_DATE_EXPIRED))
	{
		m_ui->licenseInfoLabel->setText(tr("<font color='red'>This serial number activation period is expired! Bad news.</font>"));
		return;
	}
}

}