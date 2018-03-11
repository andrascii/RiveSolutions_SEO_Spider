#pragma once

#include "iseo_spider_service_api.h"
#include "pipe_server.h"

namespace Common
{

class IIpcSignaledObject;

}

namespace SeoSpiderServiceApi
{

class PipeServer;

class SeoSpiderServiceApiImpl : public ISeoSpiderServiceApi
{
public:
	SeoSpiderServiceApiImpl();

	virtual void init() noexcept override;
	virtual void free() const noexcept override;
	virtual void setProcessSignaledState() const noexcept override;
	virtual void setProcessExceptionHandlers() const noexcept override;
	virtual void setThreadExceptionHandlers() const noexcept override;
	virtual void doAssert(const char* file, int line, const char* function, const char* expression) const noexcept override;

	virtual void traceLogMessage(
		Common::PipeMessage::Type type,
		std::uint64_t threadId,
		std::uint64_t line,
		const char* file,
		const char* function,
		const char* message) override;

	virtual void debugLogMessage(
		Common::PipeMessage::Type type,
		std::uint64_t threadId,
		std::uint64_t line,
		const char* file,
		const char* function,
		const char* message) override;

	virtual void infoLogMessage(
		Common::PipeMessage::Type type,
		std::uint64_t threadId,
		std::uint64_t line,
		const char* file,
		const char* function,
		const char* message) override;

	virtual void warningLogMessage(
		Common::PipeMessage::Type type,
		std::uint64_t threadId,
		std::uint64_t line,
		const char* file,
		const char* function,
		const char* message) override;

	virtual void errorLogMessage(
		Common::PipeMessage::Type type,
		std::uint64_t threadId,
		std::uint64_t line,
		const char* file,
		const char* function,
		const char* message) override;

private:
	void debugReport(const char* file, int line, const char* function, const char* expression) const noexcept;

	static LONG WINAPI sehHandler(PEXCEPTION_POINTERS pExceptionPtrs);
	static void pureCallHandler();
	static void terminateHandler();
	static void unexpectedHandler();
	static void sigAbortHandler(int);
	static void sigFpeHandler(int code, int subcode);
	static void sigIntHandler(int);
	static void sigIllHandler(int);
	static void sigSegvHandler(int);
	static void sigTermHandler(int);
	static int newHandler(size_t);

	static void invalidParameterHandler(
		const wchar_t* expression,
		const wchar_t* function,
		const wchar_t* file,
		unsigned int line, uintptr_t pReserved);
	
	static bool lockExceptionFilter();

private:
	static SeoSpiderServiceApiImpl* s_self;

	STARTUPINFOW m_startupInfo;
	PROCESS_INFORMATION m_processInfo;
	mutable std::mutex m_mutex;
	bool m_initialized;
	std::shared_ptr<Common::IIpcSignaledObject> m_crashEventSignaledObject;
	std::unique_ptr<PipeServer> m_pipeServer;
};

}