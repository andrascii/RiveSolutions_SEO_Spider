#include "vm_protect_license_service.h"

namespace CrawlerEngine
{

VmProtectLicenseService::VmProtectLicenseService(QObject* parent)
	: QObject(parent)
{
}

SerialNumberStates VmProtectLicenseService::setSerialNumber(const QByteArray& serialNumber)
{
	SerialNumberStates states = fromVmProtectStates(VMProtectSetSerialNumber(serialNumber.data()));

	if (states.testFlag(SerialNumberState::StateSuccessActivation))
	{
		saveSerialNumberToFile(serialNumber);
	}

	return states;
}

SerialNumberData VmProtectLicenseService::serialNumberData() const
{
	VMProtectSerialNumberData data{ 0 };
	VMProtectGetSerialNumberData(&data, sizeof(data));
	return data;
}

SerialNumberStates VmProtectLicenseService::serialNumberStates() const
{
	return fromVmProtectStates(VMProtectGetSerialNumberState());
}

void VmProtectLicenseService::requestSerialNumberData(const RequesterSharedPtr&)
{
	// TODO: implement
}

}