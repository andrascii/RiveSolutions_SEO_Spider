#include "helpers.h"
#include "abstract_license_service.h"

namespace CrawlerEngine
{

bool AbstractLicenseService::saveSerialNumberToFile(const QByteArray& serialNumber) const
{
	QFile serialNumberFile(Common::Helpers::serialNumberFilePath());

	if (!serialNumberFile.open(QIODevice::WriteOnly))
	{
		return false;
	}

	serialNumberFile.write(serialNumber);

	return true;
}

}