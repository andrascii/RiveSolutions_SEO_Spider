#include "serial_number_data.h"

namespace CrawlerEngine
{

bool operator==(const SerialNumberData& lhs, const SerialNumberData& rhs)
{
	if (std::addressof(lhs) == std::addressof(rhs))
	{
		return true;
	}

	return lhs.states == rhs.states &&
		lhs.userName == rhs.userName &&
		lhs.email == rhs.email &&
		lhs.dateExpire == rhs.dateExpire &&
		lhs.dateMaxBuild == rhs.dateMaxBuild &&
		lhs.runningTime == rhs.runningTime &&
		lhs.userDataLength == rhs.userDataLength &&
		std::equal(std::begin(lhs.userData), std::begin(lhs.userData) + lhs.userDataLength, std::begin(rhs.userData));
}

bool operator!=(const SerialNumberData& lhs, const SerialNumberData& rhs)
{
	return !(lhs == rhs);
}

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