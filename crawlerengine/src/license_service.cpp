#include "license_service.h"
#include "set_serial_number_request.h"
#include "set_serial_number_response.h"
#include "get_serial_number_data_request.h"
#include "get_serial_number_data_response.h"
#include "get_serial_number_state_request.h"
#include "get_serial_number_state_response.h"
#include "license_state.h"

namespace CrawlerEngine
{

LicenseService::LicenseService()
	: m_isTrialLicense(false)
{
	m_licenseRequester.reset(GetSerialNumberDataRequest(), this, &LicenseService::onLicenseData);
	m_licenseRequester->start();
}

bool LicenseService::isPayedLicense() const noexcept
{
	return !isTrialLicense();
}

bool LicenseService::isTrialLicense() const noexcept
{
	return m_isTrialLicense;
}

void LicenseService::onLicenseData(Requester*, const GetSerialNumberDataResponse& response)
{
	m_licenseRequester.reset();

	VMProtectSerialNumberData data = response.data();
	LicenseStateFlags stateFlags(data.nState);

	if (stateFlags.testFlag(SERIAL_STATE_SUCCESS))
	{
		setTrialLicense(false);
	}
	else if (stateFlags.testFlag(SERIAL_STATE_FLAG_INVALID))
	{
		setTrialLicense(true);
	}
}

void LicenseService::setTrialLicense(bool value)
{
	if (m_isTrialLicense == value)
	{
		return;
	}

	m_isTrialLicense = value;

	emit licenseChanged();
}

}