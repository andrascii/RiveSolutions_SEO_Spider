#pragma once

#include "serial_number_data.h"
#include "requester.h"

namespace CrawlerEngine
{

class ILicenseService
{
public:
	virtual ~ILicenseService() = default;

	virtual SerialNumberStates setSerialNumber(const QByteArray& serialNumber) = 0;
	virtual SerialNumberData serialNumberData() const = 0;
	virtual SerialNumberStates serialNumberStates() const = 0;
	virtual void requestSerialNumberData(const RequesterSharedPtr& requester) = 0;
};

}