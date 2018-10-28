#include "abstract_license_service.h"

namespace CrawlerEngine
{

bool AbstractLicenseService::saveSerialNumberToFile(const QByteArray& serialNumber) const
{
	QFile serialNumberFile("serial.txt");

	if (!serialNumberFile.open(QIODevice::WriteOnly))
	{
		return false;
	}

	serialNumberFile.write(serialNumber);

	return true;
}

}