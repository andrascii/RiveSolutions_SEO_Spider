#pragma once

#include "pipe_message.h"

namespace SeoSpiderServiceApi
{

class ISeoSpiderServiceApi
{
public:
	virtual void init() noexcept = 0;
	virtual void free() const noexcept = 0;
	
	virtual void setProcessSignaledState() const noexcept = 0;
	virtual void setProcessExceptionHandlers() const noexcept = 0;
	virtual void setThreadExceptionHandlers() const noexcept = 0;

	virtual void doAssert(const char* file, int line, const char* function, const char* expression) const noexcept = 0;

	virtual void traceLogMessage(
		Common::PipeMessage::Type type, 
		std::uint64_t threadId, 
		std::uint64_t line,
		const char* file,
		const char* function,
		const char* message) = 0;

	virtual void debugLogMessage(
		Common::PipeMessage::Type type,
		std::uint64_t threadId,
		std::uint64_t line,
		const char* file,
		const char* function,
		const char* message) = 0;

	virtual void infoLogMessage(
		Common::PipeMessage::Type type,
		std::uint64_t threadId,
		std::uint64_t line,
		const char* file,
		const char* function,
		const char* message) = 0;

	virtual void warningLogMessage(
		Common::PipeMessage::Type type,
		std::uint64_t threadId,
		std::uint64_t line,
		const char* file,
		const char* function,
		const char* message) = 0;

	virtual void errorLogMessage(
		Common::PipeMessage::Type type,
		std::uint64_t threadId,
		std::uint64_t line,
		const char* file,
		const char* function,
		const char* message) = 0;
};

}