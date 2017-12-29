#pragma once

#include "iipc_signaled_object.h"

namespace Common
{

class IpcSignaledObjectUnixLike : public IIpcSignaledObject
{
public:
	virtual bool create(const std::wstring&) noexcept override
	{
		return false;
	}
	virtual bool open(const std::wstring&) noexcept override
	{
		return false;
	}
	virtual bool isNull() const noexcept override
	{
		return true;
	}
	virtual void setSignaledState() noexcept override
	{
	}
	virtual void resetState() noexcept override
	{
	}
	virtual WakeupReason waitFor(std::uint32_t) noexcept override
	{
		return IIpcSignaledObject::WakeupReason::TimeoutReason;
	}
};

}