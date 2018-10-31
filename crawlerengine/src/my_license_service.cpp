#include "my_license_service.h"
#include "download_response.h"
#include "download_request.h"
#include "helpers.h"

namespace
{

using namespace CrawlerEngine;

constexpr int c_myServiceRawDataParts = 3;
constexpr int c_hour = 1000;// 60 * 60 * 1000;

const QString s_verifySerialNumberUrl("https://pay.rivesolutions.com");
const QString s_verifyPageName("verifykey");
const QString s_userNameVariableName("username");
const QString s_idVariableName("id");
const QString s_macVariableName("mac");
const QString s_secretVariableName("secret");

const QString s_statusKey("status");
const QString s_statusOk("ok");
const QString s_statusBlocked("blocked");
const QString s_statusUserNotFound("user_not_found");

Url makeVerifyUrl(const QString& userName, const QString& id, const QString& mac, const QString& secret)
{
	QUrlQuery urlQuery;
	urlQuery.addQueryItem(s_userNameVariableName, userName);
	urlQuery.addQueryItem(s_idVariableName, id);
	urlQuery.addQueryItem(s_macVariableName, mac);
	urlQuery.addQueryItem(s_secretVariableName, secret);

	Url url(s_verifySerialNumberUrl + "/" + s_verifyPageName);
	url.setQuery(urlQuery);

	auto var = url.toDisplayString();
	var;

	return url;
}

SerialNumberStates statesByDate(const SerialNumberData& data)
{
	SerialNumberStates states;

	if (QDateTime::currentDateTime() < data.dateExpire)
	{
		states.setFlag(SerialNumberState::StateSuccessActivation);
	}
	else
	{
		states.setFlag(SerialNumberState::StateDateExpired);
		states.setFlag(SerialNumberState::StateMaxBuildExpired);
	}

	return states;
}

SerialNumberStates statesByData(const SerialNumberData& data)
{
	if (data == SerialNumberData())
	{
		return SerialNumberState::StateInvalidSerialNumberActivation;
	}

	return statesByDate(data);
}

SerialNumberData parseMyLicenseSerialNumber(const QByteArray& serialNumber)
{
	const Common::Helpers::ParsedSerialNumberData parsedData = Common::Helpers::parseMySerialNumber(serialNumber);

	const QByteArray& id = std::get<0>(parsedData);
	const QByteArray& userName = std::get<1>(parsedData);
	const QDate& dateExpire = std::get<2>(parsedData);

	if (dateExpire.isNull())
	{
		return SerialNumberData();
	}

	SerialNumberData data;
	data.dateExpire = QDateTime(dateExpire, QTime());
	data.dateMaxBuild = QDateTime(dateExpire, QTime());
	data.userName = userName;
	data.userDataLength = id.size();
	std::copy(id.begin(), id.end(), reinterpret_cast<char*>(&data.userData));
	data.userData[id.size()] = 0;

	return data;
}

}

namespace CrawlerEngine
{

MyLicenseService::MyLicenseService(QObject* parent)
	: QObject(parent)
{
	ASSERT(startTimer(c_hour));
}

SerialNumberStates MyLicenseService::setSerialNumber(const QByteArray& serialNumber)
{
	m_data = parseMyLicenseSerialNumber(serialNumber);
	m_data.states = statesByData(m_data);

	if (m_data.states.testFlag(SerialNumberState::StateSuccessActivation))
	{
		m_validSerialNumber = serialNumber;
		saveSerialNumberToFile(serialNumber);
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

void MyLicenseService::timerEvent(QTimerEvent*)
{
	if (!m_data.states.testFlag(SerialNumberState::StateSuccessActivation))
	{
		return;
	}

	if (QDateTime::currentDateTime() > m_data.dateExpire)
	{
		m_data.states = SerialNumberStates();
		m_data.states.setFlag(SerialNumberState::StateDateExpired);
		return;
	}

	const QByteArray hashedSerialNumber = QCryptographicHash::hash(m_validSerialNumber, QCryptographicHash::Md5);

	const Url verifyUrl = makeVerifyUrl(m_data.userName,
		QString::fromUtf8(reinterpret_cast<char*>(&m_data.userData)),
		Common::Helpers::macAddress(),
		hashedSerialNumber);

	const CrawlerRequest crawlerRequest{ verifyUrl, DownloadRequestType::RequestTypeGet };

	DownloadRequest request(crawlerRequest, DownloadRequest::LinkStatus::LinkStatusFirstLoading,
		DownloadRequest::BodyProcessingCommand::CommandAutoDetectionBodyLoadingNecessity, true);

	m_verifyKeyRequester.reset(request, this, &MyLicenseService::onLoadingDone);
	m_verifyKeyRequester->start();
}

void MyLicenseService::onLoadingDone(Requester*, const DownloadResponse& response)
{
	m_verifyKeyRequester.reset();

	if (response.hopsChain.empty())
	{
		INFOLOG << "Empty hops chain";
		return;
	}

	if (response.hopsChain.lastHop().statusCode() != Common::StatusCode::Ok200)
	{
		INFOLOG << "Last hop status code:" << static_cast<int>(response.hopsChain.lastHop().statusCode());
		return;
	}

	QJsonDocument jsonDocument = QJsonDocument::fromJson(response.hopsChain.lastHop().body());
	const QString statusValue = jsonDocument[s_statusKey].toString();

	const SerialNumberStates states = statesByDate(m_data);

	if (states.testFlag(SerialNumberState::StateSuccessActivation) && statusValue == s_statusOk)
	{
		m_data.states = SerialNumberStates();
		m_data.states.setFlag(SerialNumberState::StateSuccessActivation);
	}
	else if (statusValue == s_statusUserNotFound)
	{
		m_data.states = SerialNumberStates();
		m_data.states.setFlag(SerialNumberState::StateInvalidSerialNumberActivation);
	}
	else if (statusValue == s_statusBlocked)
	{
		m_data.states = SerialNumberStates();
		m_data.states.setFlag(SerialNumberState::StateSerialNumberBlacklisted);
	}
}

}