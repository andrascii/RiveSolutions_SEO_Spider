#pragma once

#include "iwatch_dog_service_api.h"

namespace WatchDogApi
{

class WatchDogServiceApi : public IWatchDogServiceApi
{
public:
	WatchDogServiceApi();

	virtual void init() noexcept override;
	virtual void free() const noexcept override;
	virtual void setProcessExceptionHandlers() const noexcept override;
	virtual void setThreadExceptionHandlers() const noexcept override;

private:
	HANDLE m_crashEventHandle;
	STARTUPINFOW m_startupInfo;
	PROCESS_INFORMATION m_processInfo;
};

}