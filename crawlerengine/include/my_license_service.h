#pragma once

#include "abstract_license_service.h"
#include "serial_number_data.h"

namespace CrawlerEngine
{

class MyLicenseService : public QObject, public AbstractLicenseService
{
	Q_OBJECT

public:
	MyLicenseService(QObject* parent = nullptr);

	virtual SerialNumberStates setSerialNumber(const QByteArray& serialNumber) override;
	virtual SerialNumberData serialNumberData() const override;
	virtual SerialNumberStates serialNumberStates() const override;

private:
	SerialNumberData parseMyLicenseSerialNumber(const QByteArray& serialNumber) const;

private:
	SerialNumberData m_data;
};

}