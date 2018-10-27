#pragma once

namespace CrawlerEngine
{

enum SerialNumberState
{
	StateSuccessActivation,
	StateDateExpired, // possibly invalid serial number
	StateMaxBuildExpired,
	StateRunningTimeOver, // when license has ended (i.e. the launch of the program the day after the license ended)
	StateInvalidSerialNumberActivation,
	StateSerialNumberBlacklisted
};

Q_DECLARE_FLAGS(SerialNumberStates, SerialNumberState);

struct SerialNumberData
{
	SerialNumberData()
	{
		states.setFlag(SerialNumberState::StateInvalidSerialNumberActivation);
	}

	SerialNumberData(const VMProtectSerialNumberData& data)
	{
		if (data.nState & SERIAL_STATE_SUCCESS)
		{
			states.setFlag(SerialNumberState::StateSuccessActivation);
		}
		if (data.nState & SERIAL_STATE_FLAG_INVALID)
		{
			states.setFlag(SerialNumberState::StateInvalidSerialNumberActivation);
		}
		if (data.nState & SERIAL_STATE_FLAG_BLACKLISTED)
		{
			states.setFlag(SerialNumberState::StateSerialNumberBlacklisted);
		}
		if (data.nState & SERIAL_STATE_FLAG_DATE_EXPIRED)
		{
			states.setFlag(SerialNumberState::StateDateExpired);
		}
		if (data.nState & SERIAL_STATE_FLAG_RUNNING_TIME_OVER)
		{
			states.setFlag(SerialNumberState::StateRunningTimeOver);
		}
		if (data.nState & SERIAL_STATE_FLAG_MAX_BUILD_EXPIRED)
		{
			states.setFlag(SerialNumberState::StateMaxBuildExpired);
		}

		userName = QString::fromWCharArray(data.wUserName);
		email = QString::fromWCharArray(data.wEMail);
		dateExpire = QDateTime(QDate(data.dtExpire.wYear, data.dtExpire.bMonth, data.dtExpire.bDay), QTime());
		dateExpire = QDateTime(QDate(data.dtMaxBuild.wYear, data.dtMaxBuild.bMonth, data.dtMaxBuild.bDay), QTime());
		runningTime = data.bRunningTime;
	}

	SerialNumberStates states;
	QString userName;
	QString email;
	QDateTime dateExpire;
	QDateTime dateMaxBuild;
	int runningTime;
	unsigned char userDataLength;
	unsigned char userData[255];
};

}