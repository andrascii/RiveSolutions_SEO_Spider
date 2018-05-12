#pragma once

#include "requester_wrapper.h"

namespace CrawlerEngine
{

class GetSerialNumberDataResponse;
class Requester;

class ILicenseService
{
public:
	virtual bool isPayedLicense() const noexcept = 0;
	virtual bool isTrialLicense() const noexcept = 0;

	// signals
	virtual void licenseChanged() const = 0;
};

class LicenseService : public QObject, public ILicenseService
{
	Q_OBJECT

public:
	LicenseService();

	virtual bool isPayedLicense() const noexcept override;
	virtual bool isTrialLicense() const noexcept override;

	// signals
	Q_SIGNAL virtual void licenseChanged() const override;

private:
	void onLicenseData(Requester* requester, const GetSerialNumberDataResponse& response);
	void setTrialLicense(bool value);

private:
	bool m_isTrialLicense;
	RequesterWrapper m_licenseRequester;
};

}