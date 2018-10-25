#pragma once

#include "ilicense_service.h"

namespace CrawlerEngine
{

class MyLicenseService : public QObject, public ILicenseService
{
	Q_OBJECT

public:
	MyLicenseService(QObject* parent = nullptr);

	virtual SerialNumberStates setSerialNumber(const QByteArray& serialNumber) override;
	virtual SerialNumberData serialNumberData() const override;
	virtual SerialNumberStates serialNumberStates() const override;
};

}