#pragma once

#include "iwatch_dog_service_api.h"

namespace WatchDog
{

class WatchDogServiceApi : public IWatchDogServiceApi
{
public:
	virtual void setProcessExceptionHandlers() const noexcept override;
	virtual void setThreadExceptionHandlers() const noexcept override;
};

}