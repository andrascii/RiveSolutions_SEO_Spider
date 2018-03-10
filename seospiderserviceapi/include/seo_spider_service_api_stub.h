#pragma once

#include "iseo_spider_service_api.h"

namespace SeoSpiderServiceApi
{

class SeoSpiderServiceApiStub : public ISeoSpiderServiceApi
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
	virtual void doAssert(const char*, int, const char*, const char*) const noexcept override
	{
	}
	virtual void traceLogMessage(
		Common::PipeMessage::Type,
		std::uint64_t,
		std::uint64_t,
		const char*,
		const char*,
		const char*) override
	{
	}
	virtual void debugLogMessage(
		Common::PipeMessage::Type,
		std::uint64_t,
		std::uint64_t,
		const char*,
		const char*,
		const char*) override
	{
	}
	virtual void infoLogMessage(
		Common::PipeMessage::Type,
		std::uint64_t,
		std::uint64_t,
		const char*,
		const char*,
		const char*) override
	{
	}
	virtual void warningLogMessage(
		Common::PipeMessage::Type,
		std::uint64_t,
		std::uint64_t,
		const char*,
		const char*,
		const char*) override
	{
	}
	virtual void errorLogMessage(
		Common::PipeMessage::Type,
		std::uint64_t,
		std::uint64_t,
		const char*,
		const char*,
		const char*) override
	{
	}
};

}