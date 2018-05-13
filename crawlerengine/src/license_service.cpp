#include "license_service.h"
#include "iresponse.h"
#include "set_serial_number_request.h"
#include "set_serial_number_response.h"
#include "get_serial_number_data_request.h"
#include "get_serial_number_data_response.h"
#include "get_serial_number_state_request.h"
#include "get_serial_number_state_response.h"
#include "handler_registry.h"

namespace CrawlerEngine
{

LicenseService::LicenseService()
	: m_isTrialLicense(false)
{
	HandlerRegistry& handlerRegistry = HandlerRegistry::instance();

	QObject* getSerialNumberDataRequestHandler =
		handlerRegistry.handlerForRequest(RequestType::RequestGetSerialNumberData);

	QObject* getSerialNumberStateRequestHandler =
		handlerRegistry.handlerForRequest(RequestType::RequestGetSerialNumberState);

	QObject* setSerialNumberRequestHandler =
		handlerRegistry.handlerForRequest(RequestType::RequestSetSerialNumber);

	ASSERT(getSerialNumberDataRequestHandler == getSerialNumberStateRequestHandler &&
		getSerialNumberStateRequestHandler == setSerialNumberRequestHandler);

	const auto subscription = [this](const IResponse& response)
	{
		onSubscription(response);
	};

	handlerRegistry.addSubscription(subscription, getSerialNumberDataRequestHandler, ResponseType::ResponseGetSerialNumberData);
	handlerRegistry.addSubscription(subscription, getSerialNumberStateRequestHandler, ResponseType::ResponseGetSerialNumberState);
	handlerRegistry.addSubscription(subscription, setSerialNumberRequestHandler, ResponseType::ResponseSetSerialNumber);

	m_licenseRequester.reset(GetSerialNumberDataRequest(), this, &LicenseService::onLicenseData);
	m_licenseRequester->start();
}

QObject* LicenseService::qobject() const
{
	return const_cast<LicenseService*>(this);
}

bool LicenseService::isPaidLicense() const noexcept
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

	const LicenseStateFlags stateFlags(response.data().nState);

	onLicenseStateChanged(stateFlags);
}

void LicenseService::onLicenseStateChanged(const LicenseStateFlags& stateFlags)
{
	if (stateFlags.testFlag(SERIAL_STATE_SUCCESS))
	{
		setTrialLicense(false);
	}
	else if (stateFlags.testFlag(SERIAL_STATE_FLAG_INVALID))
	{
		setTrialLicense(true);
	}
}

void LicenseService::onSubscription(const IResponse& response)
{
	switch (response.type())
	{
		case ResponseType::ResponseSetSerialNumber:
		{
			const SetSerialNumberResponse& setSerialNumberResponse = 
				static_cast<const SetSerialNumberResponse&>(response);

			onLicenseStateChanged(setSerialNumberResponse.state());

			break;
		}
		case ResponseType::ResponseGetSerialNumberData:
		{
			const GetSerialNumberDataResponse& getSerialNumberDataResponse =
				static_cast<const GetSerialNumberDataResponse&>(response);

			onLicenseStateChanged(QFlag(getSerialNumberDataResponse.data().nState));

			break;
		}
		case ResponseType::ResponseGetSerialNumberState:
		{
			const GetSerialNumberStateResponse& getSerialNumberStateResponse =
				static_cast<const GetSerialNumberStateResponse&>(response);

			onLicenseStateChanged(getSerialNumberStateResponse.state());

			break;
		}
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