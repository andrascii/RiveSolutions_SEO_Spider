#include "watch_dog_service_api.h"
#include "crash_handler.h"

namespace WatchDogApi
{


WatchDogServiceApi::WatchDogServiceApi()
	: m_startupInfo{ sizeof(m_startupInfo) }
{
}

void WatchDogServiceApi::init() noexcept
{
	const boost::uuids::uuid uuid = boost::uuids::random_generator()();
	const std::wstring crashHandlerEventName = boost::lexical_cast<std::wstring>(uuid);

	std::wstring commandLine = L"watchdogservice.exe " + crashHandlerEventName;

	m_crashEventHandle = CreateEventW(nullptr, FALSE, FALSE, crashHandlerEventName.c_str());

	CreateProcessW(
		NULL,
		commandLine.data(),
		NULL,
		NULL,
		TRUE,
		CREATE_NO_WINDOW,
		NULL,
		NULL,
		&m_startupInfo,
		&m_processInfo
	);
}


void WatchDogServiceApi::free() const noexcept
{
	TerminateProcess(m_processInfo.hProcess, 0);

	CloseHandle(m_processInfo.hThread);
	CloseHandle(m_processInfo.hProcess);
}

void WatchDogServiceApi::setProcessExceptionHandlers() const noexcept
{
	SetUnhandledExceptionFilter(CrashHandler::sehHandler);

#ifdef _WIN64

	AddVectoredExceptionHandler(0, CrashHandler::stackOverflowExceptionFilter);

#endif

	_set_purecall_handler(CrashHandler::pureCallHandler);

	_set_new_handler(CrashHandler::newHandler);

	_set_invalid_parameter_handler(CrashHandler::invalidParameterHandler);

	_set_abort_behavior(_CALL_REPORTFAULT, _CALL_REPORTFAULT);

	std::signal(SIGABRT, CrashHandler::sigAbortHandler);

	std::signal(SIGINT, CrashHandler::sigIntHandler);

	std::signal(SIGTERM, CrashHandler::sigTermHandler);
}

void WatchDogServiceApi::setThreadExceptionHandlers() const noexcept
{
	typedef void(*SignalHandlerFunctionType)(int);

	set_terminate(CrashHandler::terminateHandler);

	set_unexpected(CrashHandler::unexpectedHandler);

	std::signal(SIGFPE, reinterpret_cast<SignalHandlerFunctionType>(CrashHandler::sigFpeHandler));

	std::signal(SIGILL, CrashHandler::sigIllHandler);

	std::signal(SIGSEGV, CrashHandler::sigSegvHandler);
}

}