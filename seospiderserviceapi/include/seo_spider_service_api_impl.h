#pragma once

#include "iseo_spider_service_api.h"
#include "pipe_server.h"
#include "log_filter.h"

namespace Common
{

class IIpcSignaledObject;

}

namespace SeoSpiderServiceApi
{

class SeoSpiderServiceApiImpl : public ISeoSpiderServiceApi
{
public:
	SeoSpiderServiceApiImpl();

	virtual void init() override;
	virtual void free() const override;
	virtual bool restartApplication(int msec) override;
	virtual bool doDump(const void* exceptionInfo, const int exceptionInfoSize) override;
	virtual void setProcessExceptionHandlers() const override;
	virtual void setThreadExceptionHandlers() const override;
	virtual void doAssert(const char* file, int line, const char* function, const char* expression) override;
	virtual void setLogFilter(const std::function<bool(Common::LogLevel)>& filter) override;
	virtual void commitCounterData(const char* name, quint64 value, int counterType) override;

	virtual bool writeLog(
		int id,
		int level,
		const char* file,
		unsigned int line,
		const char* function,
		const void* thisptr,
		const char* text) override;

private:
	void debugReport(const char* file, int line, const char* function, const char* expression) const;

	static LONG WINAPI exceptionFilter(PEXCEPTION_POINTERS pExceptionPtrs);
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

	unsigned assertExceptionFilter(const char* file, int line, const char* function,
		const void* thisptr, const char* expression, void* exceptionInfo);

	bool writeLogStack(int id, int level, const char* file, unsigned int line,
		const char* function, const void* thisptr, const char* text);
	bool writeLogHeap(int id, int level, const char* file, unsigned int line,
		const char* function, const void* thisptr, const char* text);

	bool makeCrashDump(const void* exceptionInfo, const int exceptionInfoSize);

private:
	static SeoSpiderServiceApiImpl* s_self;

	mutable std::mutex m_mutex;
	STARTUPINFOW m_startupInfo;
	PROCESS_INFORMATION m_processInfo;
	std::atomic_bool m_initialized;
	std::unique_ptr<Common::PipeServer> m_pipeServer;
	LogFilter m_logFilter;
};

}