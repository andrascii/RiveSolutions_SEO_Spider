#pragma once

#include "abstract_license_service.h"

namespace CrawlerEngine
{

class VmProtectLicenseService : public QObject, public AbstractLicenseService
{
	Q_OBJECT

public:
	VmProtectLicenseService(QObject* parent = nullptr);

	virtual SerialNumberStates setSerialNumber(const QByteArray& serialNumber) override;
	virtual SerialNumberData serialNumberData() const override;
	virtual SerialNumberStates serialNumberStates() const override;
};

}