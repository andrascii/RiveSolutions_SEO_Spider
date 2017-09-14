#include "crash_handler.h"

namespace WatchDogApi
{

LONG WINAPI CrashHandler::sehHandler(PEXCEPTION_POINTERS pExceptionPtrs)
{
	setEventToWatchDogProcess();
	return EXCEPTION_EXECUTE_HANDLER;
}

LONG WINAPI CrashHandler::vehHandler(EXCEPTION_POINTERS* exceptionInfo)
{
	return sehHandler(exceptionInfo);
}

void CrashHandler::terminateHandler()
{
	setEventToWatchDogProcess();
}

void CrashHandler::unexpectedHandler()
{
	setEventToWatchDogProcess();
}

void CrashHandler::pureCallHandler()
{
	setEventToWatchDogProcess();
}

void CrashHandler::sigAbortHandler(int)
{
	setEventToWatchDogProcess();
}

void CrashHandler::sigFpeHandler(int code, int subcode)
{
	setEventToWatchDogProcess();
}

void CrashHandler::sigIntHandler(int)
{
	setEventToWatchDogProcess();
}

void CrashHandler::sigIllHandler(int)
{
	setEventToWatchDogProcess();
}

void CrashHandler::sigSegvHandler(int)
{
	setEventToWatchDogProcess();
}

void CrashHandler::sigTermHandler(int)
{
	setEventToWatchDogProcess();
}

int CrashHandler::newHandler(size_t)
{
	setEventToWatchDogProcess();
	return 0;
}

void CrashHandler::invalidParameterHandler(
	const wchar_t* expression, 
	const wchar_t* function,
	const wchar_t* file, 
	unsigned int line, 
	uintptr_t pReserved)
{
	setEventToWatchDogProcess();
}

void CrashHandler::setEventToWatchDogProcess()
{
	MessageBoxA(NULL, "Handler intercepted critical error", "Crash", MB_OK);
	TerminateProcess(GetCurrentProcess(), 0xDEAD);
}

}