#pragma once

#include "requester_wrapper.h"
#include "license_state.h"

namespace CrawlerEngine
{

class GetSerialNumberDataResponse;
class Requester;

class ILicenseService
{
public:
	virtual ~ILicenseService() = default; 
	
	virtual QObject* qobject() const = 0;

	virtual QObject* qobject() const = 0;

	virtual bool isPaidLicense() const noexcept = 0;
	virtual bool isTrialLicense() const noexcept = 0;

	// signals
	virtual void licenseChanged() const = 0;
};

class LicenseService : public QObject, public ILicenseService
{
	Q_OBJECT

public:
	LicenseService();

	virtual QObject* qobject() const override;

	virtual bool isPaidLicense() const noexcept override;
	virtual bool isTrialLicense() const noexcept override;

	// signals
	Q_SIGNAL virtual void licenseChanged() const override;

private:
	void onLicenseData(Requester* requester, const GetSerialNumberDataResponse& response);
	void onLicenseStateChanged(const LicenseStateFlags& stateFlags);
	void onSubscription(const IResponse& response);
	void setTrialLicense(bool value);

private:
	bool m_isTrialLicense;
	RequesterWrapper m_licenseRequester;
};

}