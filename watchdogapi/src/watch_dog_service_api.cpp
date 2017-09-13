#include "watch_dog_service_api.h"
#include "crash_handler.h"

namespace WatchDog
{

void WatchDogServiceApi::setProcessExceptionHandlers() const noexcept
{
	// Install top-level SEH handler
	SetUnhandledExceptionFilter(CrashHandler::sehHandler);

	// Catch pure virtual function calls.
	// Because there is one _purecall_handler for the whole process, 
	// calling this function immediately impacts all threads. The last 
	// caller on any thread sets the handler. 
	// http://msdn.microsoft.com/en-us/library/t296ys27.aspx
	_set_purecall_handler(CrashHandler::pureCallHandler);

	// Catch new operator memory allocation exceptions
	_set_new_handler(CrashHandler::newHandler);

	// Catch invalid parameter exceptions.
	_set_invalid_parameter_handler(CrashHandler::invalidParameterHandler);

	// Set up C++ signal handlers

	_set_abort_behavior(_CALL_REPORTFAULT, _CALL_REPORTFAULT);

	// Catch an abnormal program termination
	std::signal(SIGABRT, CrashHandler::sigAbortHandler);

	// Catch illegal instruction handler
	std::signal(SIGINT, CrashHandler::sigIntHandler);

	// Catch a termination request
	std::signal(SIGTERM, CrashHandler::sigTermHandler);
}

void WatchDogServiceApi::setThreadExceptionHandlers() const noexcept
{
	// Catch terminate() calls. 
	// In a multithreaded environment, terminate functions are maintained 
	// separately for each thread. Each new thread needs to install its own 
	// terminate function. Thus, each thread is in charge of its own termination handling.
	// http://msdn.microsoft.com/en-us/library/t6fk7h29.aspx
	set_terminate(CrashHandler::terminateHandler);

	// Catch unexpected() calls.
	// In a multithreaded environment, unexpected functions are maintained 
	// separately for each thread. Each new thread needs to install its own 
	// unexpected function. Thus, each thread is in charge of its own unexpected handling.
	// http://msdn.microsoft.com/en-us/library/h46t5b69.aspx  
	set_unexpected(CrashHandler::unexpectedHandler);

	// Catch a floating point error
	typedef void(*sigh)(int);
	std::signal(SIGFPE, (sigh)CrashHandler::sigFpeHandler);

	// Catch an illegal instruction
	std::signal(SIGILL, CrashHandler::sigIllHandler);

	// Catch illegal storage access errors
	std::signal(SIGSEGV, CrashHandler::sigSegvHandler);
}

void WatchDogServiceApi::createMiniDump() const noexcept
{
}

}