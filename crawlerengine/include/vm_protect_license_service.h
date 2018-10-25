#pragma once

#include "ilicense_service.h"

namespace CrawlerEngine
{

class VmProtectLicenseService : public QObject, public ILicenseService
{
	Q_OBJECT

public:
	VmProtectLicenseService(QObject* parent = nullptr);

	virtual SerialNumberStates setSerialNumber(const QByteArray& serialNumber) override;
	virtual SerialNumberData serialNumberData() const override;
	virtual SerialNumberStates serialNumberStates() const override;
};

}