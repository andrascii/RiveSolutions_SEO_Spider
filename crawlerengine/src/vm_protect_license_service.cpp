#include "vm_protect_license_service.h"

namespace CrawlerEngine
{

VmProtectLicenseService::VmProtectLicenseService(QObject* parent)
	: QObject(parent)
{
}

SerialNumberStates VmProtectLicenseService::setSerialNumber(const QByteArray& serialNumber)
{
	return QFlag(VMProtectSetSerialNumber(serialNumber.data()));
}

SerialNumberData VmProtectLicenseService::serialNumberData() const
{
	VMProtectSerialNumberData data{ 0 };
	VMProtectGetSerialNumberData(&data, sizeof(data));
	return data;
}

SerialNumberStates VmProtectLicenseService::serialNumberStates() const
{
	return QFlag(VMProtectGetSerialNumberState());
}

}