#pragma once

#include "requester.h"
#include "requester_wrapper.h"
#include "serial_number_data.h"

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

	void checkLicenseFileAndInitializeService();
	void initLicenseServiceByKey(const QByteArray& key);
	SerialNumberStates setSerialNumberInternal(const QByteArray& serialNumber);

private:
	ILicenseService* m_licenseService;
};

}