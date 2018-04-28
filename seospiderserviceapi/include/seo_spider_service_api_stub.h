#pragma once

#include "iseo_spider_service_api.h"

namespace SeoSpiderServiceApi
{

class SeoSpiderServiceApiStub : public ISeoSpiderServiceApi
{
public:
	virtual void init() override
	{
	}
	virtual void free() const override
	{
	}
	virtual void setProcessSignaledState() const override
	{
	}
	virtual void setProcessExceptionHandlers() const override
	{
	}
	virtual void setThreadExceptionHandlers() const override
	{
	}
	virtual void doAssert(const char*, int, const char*, const char*) const override
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