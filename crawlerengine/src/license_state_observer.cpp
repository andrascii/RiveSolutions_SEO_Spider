#include "license_state_observer.h"
#include "iresponse.h"
#include "helpers.h"
#include "set_serial_number_request.h"
#include "set_serial_number_response.h"
#include "get_serial_number_data_request.h"
#include "get_serial_number_data_response.h"
#include "get_serial_number_state_request.h"
#include "get_serial_number_state_response.h"
#include "handler_registry.h"

namespace
{

constexpr int c_minute = 1000;// 60 * 1000;

}

namespace CrawlerEngine
{

LicenseStateObserver::LicenseStateObserver()
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

	const QByteArray normalizedSubscriptionSignature = QMetaObject::normalizedSignature("onSubscription(const IResponse&)");
	const HandlerRegistry::Subscription subscription{ this, staticMetaObject.method(staticMetaObject.indexOfMethod(normalizedSubscriptionSignature)) };

	handlerRegistry.addSubscription(subscription, getSerialNumberDataRequestHandler, ResponseType::ResponseGetSerialNumberData);
	handlerRegistry.addSubscription(subscription, getSerialNumberStateRequestHandler, ResponseType::ResponseGetSerialNumberState);
	handlerRegistry.addSubscription(subscription, setSerialNumberRequestHandler, ResponseType::ResponseSetSerialNumber);

	checkLicenseFileAndInitLicenseIfNeeded();

	ASSERT(startTimer(c_minute));
}

QObject* LicenseStateObserver::qobject() const
{
	return const_cast<LicenseStateObserver*>(this);
}

bool LicenseStateObserver::isPaidLicense() const noexcept
{
	return !isTrialLicense();
}

bool LicenseStateObserver::isTrialLicense() const noexcept
{
	return m_isTrialLicense;
}

void LicenseStateObserver::timerEvent(QTimerEvent*)
{
	if (m_licenseRequester)
	{
		return;
	}

	m_licenseRequester.reset(GetSerialNumberDataRequest(), this, &LicenseStateObserver::onLicenseData);
	m_licenseRequester->start();
}

void LicenseStateObserver::onLicenseData(Requester*, const GetSerialNumberDataResponse& response)
{
	m_licenseRequester.reset();
	onLicenseStateChanged(response.data().states);
}

void LicenseStateObserver::onSetSerialNumber(Requester*, const SetSerialNumberResponse& response)
{
	m_licenseRequester.reset();
	onLicenseStateChanged(response.states());
}

void LicenseStateObserver::onLicenseStateChanged(const SerialNumberStates& stateFlags)
{
	if (stateFlags.testFlag(SerialNumberState::StateSuccessActivation))
	{
		setTrialLicense(false, ReasonSuccessActivation);
	}
	else if (stateFlags.testFlag(SerialNumberState::StateSerialNumberBlacklisted))
	{
		setTrialLicense(true, Reason::ReasonSerialNumberBlacklisted);
	}
	else if (stateFlags.testFlag(SerialNumberState::StateInvalidSerialNumberActivation))
	{
		setTrialLicense(true, Reason::ReasonInvalidSerialNumberActivation);
	}
	else if (stateFlags.testFlag(SerialNumberState::StateDateExpired))
	{
		setTrialLicense(true, Reason::ReasonDateExpired);
	}
	else if (stateFlags.testFlag(SerialNumberState::StateRunningTimeOver))
	{
		setTrialLicense(true, Reason::ReasonRunningTimeOver);
	}
}

void LicenseStateObserver::onSubscription(const IResponse& response)
{
	if (m_licenseRequester)
	{
		return;
	}

	switch (response.type())
	{
		case ResponseType::ResponseSetSerialNumber:
		{
			const SetSerialNumberResponse& setSerialNumberResponse =
				static_cast<const SetSerialNumberResponse&>(response);

			onLicenseStateChanged(setSerialNumberResponse.states());

			break;
		}
		case ResponseType::ResponseGetSerialNumberData:
		{
			const GetSerialNumberDataResponse& getSerialNumberDataResponse =
				static_cast<const GetSerialNumberDataResponse&>(response);

			onLicenseStateChanged(getSerialNumberDataResponse.data().states);

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

void LicenseStateObserver::setTrialLicense(bool value, Reason reason)
{
	if (m_isTrialLicense == value)
	{
		return;
	}

	m_isTrialLicense = value;

	if (m_isTrialLicense)
	{
		INFOLOG << "License invalidated";
	}

	emit licenseChanged(reason);
}

void LicenseStateObserver::checkLicenseFileAndInitLicenseIfNeeded()
{
	QFile serialNumberFile(Common::Helpers::serialNumberFilePath());

	if (!serialNumberFile.open(QIODevice::ReadOnly))
	{
		m_licenseRequester.reset(GetSerialNumberDataRequest(), this, &LicenseStateObserver::onLicenseData);
		m_licenseRequester->start();

		return;
	}

	m_licenseRequester.reset(SetSerialNumberRequest(serialNumberFile.readAll()), this, &LicenseStateObserver::onSetSerialNumber);
	m_licenseRequester->start();
}

}