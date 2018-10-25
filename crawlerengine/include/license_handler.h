#pragma once

#include "requester.h"
#include "requester_wrapper.h"

namespace CrawlerEngine
{

class ILicenseService;

class LicenseHandler : public QObject
{
	Q_OBJECT

public:
	LicenseHandler();
	~LicenseHandler();

	Q_INVOKABLE void handleRequest(RequesterSharedPtr requester);
	Q_INVOKABLE void stopRequestHandling(RequesterSharedPtr requester);

private:
	void setSerialNumber(const RequesterSharedPtr& requester);
	void getSerialNumberData(const RequesterSharedPtr& requester);
	void getSerialNumberState(const RequesterSharedPtr& requester);

private:
	ILicenseService* m_licenseService;
};

}