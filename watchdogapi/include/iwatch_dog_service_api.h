#pragma once

namespace WatchDog
{

class IWatchDogServiceApi
{
public:
	virtual void setProcessExceptionHandlers() const noexcept = 0;
	virtual void setThreadExceptionHandlers() const noexcept = 0;

	virtual void createMiniDump() const noexcept = 0;
};

}