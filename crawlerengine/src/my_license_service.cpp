#include "my_license_service.h"
#include "helpers.h"

namespace
{

constexpr int c_myServiceRawDataParts = 3;

}

namespace CrawlerEngine
{

MyLicenseService::MyLicenseService(QObject* parent)
	: QObject(parent)
{
}

SerialNumberStates MyLicenseService::setSerialNumber(const QByteArray& serialNumber)
{
	m_data = parseMyLicenseSerialNumber(serialNumber);
	m_data.states = SerialNumberStates();

	if (QDateTime::currentDateTime() < m_data.dateExpire)
	{
		m_data.states.setFlag(SerialNumberState::StateSuccessActivation);

		saveSerialNumberToFile(serialNumber);
	}
	else
	{
		m_data.states.setFlag(SerialNumberState::StateDateExpired);
		m_data.states.setFlag(SerialNumberState::StateMaxBuildExpired);
	}

	return m_data.states;
}

SerialNumberData MyLicenseService::serialNumberData() const
{
	return m_data;
}

SerialNumberStates MyLicenseService::serialNumberStates() const
{
	return m_data.states;
}

SerialNumberData MyLicenseService::parseMyLicenseSerialNumber(const QByteArray& serialNumber) const
{
	const std::pair<QByteArray, QDate> parsedData = Common::Helpers::parseMySerialNumber(serialNumber);

	if (parsedData.second.isNull())
	{
		return SerialNumberData();
	}

	SerialNumberData data;
	data.dateExpire = QDateTime(parsedData.second, QTime());
	data.dateMaxBuild = QDateTime(parsedData.second, QTime());
	data.userName = parsedData.first;

	return data;
}

}