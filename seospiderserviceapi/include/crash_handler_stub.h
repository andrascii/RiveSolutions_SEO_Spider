#pragma once

#include "icrash_handler.h"

namespace SeoSpiderServiceApi
{

class CrashHandlerStub : public ICrashHandler
{
public:
	virtual void init() noexcept override
	{
	}
	virtual void free() const noexcept override
	{
	}
	virtual void setProcessSignaledState() const noexcept override
	{
	}
	virtual void setProcessExceptionHandlers() const noexcept override
	{
	}
	virtual void setThreadExceptionHandlers() const noexcept override
	{
	}
};

}