#pragma once

#include "requester_wrapper.h"
#include "serial_number_data.h"

namespace CrawlerEngine
{

class Requester;
class GetSerialNumberDataResponse;
class GetSerialNumberStateResponse;
class SetSerialNumberResponse;

class ILicenseStateObserver
{
public:
	enum Reason
	{
		ReasonSuccessActivation,
		ReasonDateExpired, // possibly invalid serial number
		ReasonRunningTimeOver, // when license has ended (i.e. the launch of the program the day after the license ended)
		ReasonInvalidSerialNumberActivation
	};

	virtual ~ILicenseStateObserver() = default;

	virtual QObject* qobject() const = 0;

	virtual bool isPaidLicense() const noexcept = 0;
	virtual bool isTrialLicense() const noexcept = 0;

	// signals
	virtual void licenseChanged(int reason) const = 0;
};

class LicenseStateObserver : public QObject, public ILicenseStateObserver
{
	Q_OBJECT

public:
	LicenseStateObserver();

	virtual QObject* qobject() const override;

	virtual bool isPaidLicense() const noexcept override;
	virtual bool isTrialLicense() const noexcept override;

	// signals
	Q_SIGNAL virtual void licenseChanged(int reason) const override;

protected:
	virtual void timerEvent(QTimerEvent* event) override;

private:
	Q_INVOKABLE void onSubscription(const IResponse& response);

	void onLicenseData(Requester* requester, const GetSerialNumberDataResponse& response);
	void onSetSerialNumber(Requester* requester, const SetSerialNumberResponse& response);
	void onLicenseStateChanged(const SerialNumberStates& stateFlags);
	void setTrialLicense(bool value, Reason reason);

	void checkLicenseFileAndInitLicenseIfNeeded();

private:
	bool m_isTrialLicense;
	RequesterWrapper m_licenseRequester;
};

}