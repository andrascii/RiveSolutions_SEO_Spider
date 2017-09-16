#include "watch_dog_service_api.h"

namespace WatchDogApi
{

WatchDogServiceApi* WatchDogServiceApi::s_self = nullptr;

WatchDogServiceApi::WatchDogServiceApi()
	: m_startupInfo{ sizeof(m_startupInfo) }
{
	Q_ASSERT(s_self == nullptr && "Double WatchDogServiceApi instances detected!");

	s_self = this;
}

void WatchDogServiceApi::init() noexcept
{
	const boost::uuids::uuid uuid = boost::uuids::random_generator()();
	const std::wstring crashHandlerEventName = boost::lexical_cast<std::wstring>(uuid);

	std::wstring commandLine = L"watchdogservice.exe " + crashHandlerEventName + L" " + std::to_wstring(GetCurrentProcessId());

	m_crashEventHandle = CreateEventW(nullptr, TRUE, FALSE, crashHandlerEventName.c_str());

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
	CloseHandle(m_processInfo.hThread);
	CloseHandle(m_processInfo.hProcess);
	CloseHandle(m_crashEventHandle);
}

void WatchDogServiceApi::setProcessSignaledState() const noexcept
{
	std::lock_guard<std::mutex> lock(m_mutex);

	SetEvent(m_crashEventHandle);

	Sleep(INFINITE);
}

void WatchDogServiceApi::setProcessExceptionHandlers() const noexcept
{
	SetUnhandledExceptionFilter(WatchDogServiceApi::sehHandler);

	_set_purecall_handler(WatchDogServiceApi::pureCallHandler);
	_set_new_handler(WatchDogServiceApi::newHandler);
	_set_invalid_parameter_handler(WatchDogServiceApi::invalidParameterHandler);
	_set_abort_behavior(_CALL_REPORTFAULT, _CALL_REPORTFAULT);

	std::signal(SIGABRT, WatchDogServiceApi::sigAbortHandler);
	std::signal(SIGINT, WatchDogServiceApi::sigIntHandler);
	std::signal(SIGTERM, WatchDogServiceApi::sigTermHandler);
}

void WatchDogServiceApi::setThreadExceptionHandlers() const noexcept
{
	typedef void(*SignalHandlerFunctionType)(int);

	set_terminate(WatchDogServiceApi::terminateHandler);
	set_unexpected(WatchDogServiceApi::unexpectedHandler);

	std::signal(SIGFPE, reinterpret_cast<SignalHandlerFunctionType>(WatchDogServiceApi::sigFpeHandler));
	std::signal(SIGILL, WatchDogServiceApi::sigIllHandler);
	std::signal(SIGSEGV, WatchDogServiceApi::sigSegvHandler);
}

LONG WINAPI WatchDogServiceApi::sehHandler(PEXCEPTION_POINTERS pExceptionPtrs)
{
	s_self->setProcessSignaledState();
	return EXCEPTION_EXECUTE_HANDLER;
}

void WatchDogServiceApi::terminateHandler()
{
	s_self->setProcessSignaledState();
}

void WatchDogServiceApi::unexpectedHandler()
{
	s_self->setProcessSignaledState();
}

void WatchDogServiceApi::pureCallHandler()
{
	s_self->setProcessSignaledState();
}

void WatchDogServiceApi::sigAbortHandler(int)
{
	s_self->setProcessSignaledState();
}

void WatchDogServiceApi::sigFpeHandler(int code, int subcode)
{
	s_self->setProcessSignaledState();
}

void WatchDogServiceApi::sigIntHandler(int)
{
	s_self->setProcessSignaledState();
}

void WatchDogServiceApi::sigIllHandler(int)
{
	s_self->setProcessSignaledState();
}

void WatchDogServiceApi::sigSegvHandler(int)
{
	s_self->setProcessSignaledState();
}

void WatchDogServiceApi::sigTermHandler(int)
{
	s_self->setProcessSignaledState();
}

int WatchDogServiceApi::newHandler(size_t)
{
	s_self->setProcessSignaledState();
	return 0;
}

void WatchDogServiceApi::invalidParameterHandler(
	const wchar_t* expression,
	const wchar_t* function,
	const wchar_t* file,
	unsigned int line,
	uintptr_t pReserved)
{
	s_self->setProcessSignaledState();
}

}