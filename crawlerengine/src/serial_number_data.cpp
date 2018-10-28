#include "serial_number_data.h"

namespace CrawlerEngine
{

SerialNumberStates fromVmProtectStates(int vmProtectStates)
{
	SerialNumberStates states;

	if (vmProtectStates & SERIAL_STATE_SUCCESS)
	{
		states.setFlag(SerialNumberState::StateSuccessActivation);
	}
	if (vmProtectStates & SERIAL_STATE_FLAG_INVALID)
	{
		states.setFlag(SerialNumberState::StateInvalidSerialNumberActivation);
	}
	if (vmProtectStates & SERIAL_STATE_FLAG_BLACKLISTED)
	{
		states.setFlag(SerialNumberState::StateSerialNumberBlacklisted);
	}
	if (vmProtectStates & SERIAL_STATE_FLAG_DATE_EXPIRED)
	{
		states.setFlag(SerialNumberState::StateDateExpired);
	}
	if (vmProtectStates & SERIAL_STATE_FLAG_RUNNING_TIME_OVER)
	{
		states.setFlag(SerialNumberState::StateRunningTimeOver);
	}
	if (vmProtectStates & SERIAL_STATE_FLAG_MAX_BUILD_EXPIRED)
	{
		states.setFlag(SerialNumberState::StateMaxBuildExpired);
	}

	return states;
}

SerialNumberData::SerialNumberData(const VMProtectSerialNumberData& data)
{
	states = fromVmProtectStates(data.nState);
	userName = QString::fromWCharArray(data.wUserName);
	email = QString::fromWCharArray(data.wEMail);
	dateExpire = QDateTime(QDate(data.dtExpire.wYear, data.dtExpire.bMonth, data.dtExpire.bDay), QTime());
	dateExpire = QDateTime(QDate(data.dtMaxBuild.wYear, data.dtMaxBuild.bMonth, data.dtMaxBuild.bDay), QTime());
	runningTime = data.bRunningTime;
}

SerialNumberData::SerialNumberData()
{
	states.setFlag(SerialNumberState::StateInvalidSerialNumberActivation);
}

}