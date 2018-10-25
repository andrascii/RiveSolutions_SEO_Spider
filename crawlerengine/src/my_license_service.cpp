#include "my_license_service.h"

namespace CrawlerEngine
{

MyLicenseService::MyLicenseService(QObject* parent)
	: QObject(parent)
{
}

SerialNumberStates MyLicenseService::setSerialNumber(const QByteArray& serialNumber)
{
	serialNumber;
	return SerialNumberStates();
}

SerialNumberData MyLicenseService::serialNumberData() const
{
	return SerialNumberData();
}

SerialNumberStates MyLicenseService::serialNumberStates() const
{
	return SerialNumberStates();
}

}