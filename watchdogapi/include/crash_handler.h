#pragma once

namespace WatchDogApi
{

class CrashHandler
{
public:
	static LONG WINAPI sehHandler(PEXCEPTION_POINTERS pExceptionPtrs);
	static LONG WINAPI stackOverflowExceptionFilter(EXCEPTION_POINTERS* exceptionInfo);
	static void terminateHandler();
	static void unexpectedHandler();
	static void pureCallHandler();
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

private:
	static void setEventToWatchDogProcess();
};

}