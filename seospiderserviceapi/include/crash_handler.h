#pragma once

#include "icrash_handler.h"

namespace SeoSpiderServiceApi
{

class CrashHandler : public ICrashHandler
{
public:
	CrashHandler();

	virtual void init() noexcept override;
	virtual void free() const noexcept override;
	
	virtual void setProcessSignaledState() const noexcept override;

	virtual void setProcessExceptionHandlers() const noexcept override;
	virtual void setThreadExceptionHandlers() const noexcept override;

private:
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
	static CrashHandler* s_self;

	HANDLE m_crashEventHandle;
	STARTUPINFOW m_startupInfo;
	PROCESS_INFORMATION m_processInfo;

	mutable std::mutex m_mutex;
};

}