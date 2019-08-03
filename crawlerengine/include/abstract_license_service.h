#pragma once
#include "stdafx.h"

#include "ilicense_service.h"

namespace CrawlerEngine
{

class AbstractLicenseService : public ILicenseService
{
protected:
	bool saveSerialNumberToFile(const QByteArray& serialNumber) const;
};

}