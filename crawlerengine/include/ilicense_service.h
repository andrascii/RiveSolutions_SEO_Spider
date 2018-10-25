#pragma once

#include "serial_number_data.h"

namespace CrawlerEngine
{

class ILicenseService
{
public:
	virtual ~ILicenseService() = default;

	virtual SerialNumberStates setSerialNumber(const QByteArray& serialNumber) = 0;
	virtual SerialNumberData serialNumberData() const = 0;
	virtual SerialNumberStates serialNumberStates() const = 0;
};

}