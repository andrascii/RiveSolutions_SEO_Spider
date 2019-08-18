#pragma once

#include "ilicense_service.h"

namespace CrawlerEngine
{

class AbstractLicenseService : public ILicenseService
{
protected:
	bool saveSerialNumberToFile(const QByteArray& serialNumber) const;
};

}