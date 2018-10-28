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

SerialNumberStates fromVmProtectStates(int vmProtectStates);

struct SerialNumberData
{
	SerialNumberData();
	SerialNumberData(const VMProtectSerialNumberData& data);

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