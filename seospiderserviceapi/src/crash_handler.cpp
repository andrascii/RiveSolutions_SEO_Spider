#include "crash_handler.h"

namespace
{

const std::uint8_t s_interceptFunctionOpcodes[] =
{
#ifdef _WIN64
	//	XOR RAX, RAX
	//	RET
	0x48, 0x31, 0xC0, 0xC3
#else
	//	XOR EAX, EAX
	//	RET 4
	0x33, 0xC0, 0xC2, 0x04, 0x00
#endif
};

}

namespace SeoSpiderServiceApi
{

CrashHandler* CrashHandler::s_self = nullptr;

CrashHandler::CrashHandler()
	: m_startupInfo{ sizeof(m_startupInfo) }
{
	Q_ASSERT(s_self == nullptr && "Double CrashHandler instances detected!");

	s_self = this;
}

void CrashHandler::init() noexcept
{
	const boost::uuids::uuid uuid = boost::uuids::random_generator()();
	const std::wstring crashHandlerEventName = boost::lexical_cast<std::wstring>(uuid);

	std::wstring commandLine = L"seospiderservice.exe " + crashHandlerEventName + L" " + std::to_wstring(GetCurrentProcessId());

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

void CrashHandler::free() const noexcept
{
	CloseHandle(m_processInfo.hThread);
	CloseHandle(m_processInfo.hProcess);
	CloseHandle(m_crashEventHandle);
}

void CrashHandler::setProcessSignaledState() const noexcept
{
	std::lock_guard<std::mutex> lock(m_mutex);

	SetEvent(m_crashEventHandle);

	Sleep(INFINITE);
}

void CrashHandler::setProcessExceptionHandlers() const noexcept
{
	SetUnhandledExceptionFilter(CrashHandler::sehHandler);
	lockExceptionFilter();

	_set_purecall_handler(CrashHandler::pureCallHandler);
	_set_new_handler(CrashHandler::newHandler);
	_set_invalid_parameter_handler(CrashHandler::invalidParameterHandler);
	_set_abort_behavior(_CALL_REPORTFAULT, _CALL_REPORTFAULT);

	std::signal(SIGABRT, CrashHandler::sigAbortHandler);
	std::signal(SIGINT, CrashHandler::sigIntHandler);
	std::signal(SIGTERM, CrashHandler::sigTermHandler);
}

void CrashHandler::setThreadExceptionHandlers() const noexcept
{
	typedef void(*SignalHandlerFunctionType)(int);

	set_terminate(CrashHandler::terminateHandler);
	set_unexpected(CrashHandler::unexpectedHandler);

	std::signal(SIGFPE, reinterpret_cast<SignalHandlerFunctionType>(CrashHandler::sigFpeHandler));
	std::signal(SIGILL, CrashHandler::sigIllHandler);
	std::signal(SIGSEGV, CrashHandler::sigSegvHandler);
}

LONG WINAPI CrashHandler::sehHandler(PEXCEPTION_POINTERS pExceptionPtrs)
{
	s_self->setProcessSignaledState();
	return EXCEPTION_EXECUTE_HANDLER;
}

void CrashHandler::terminateHandler()
{
	s_self->setProcessSignaledState();
}

void CrashHandler::unexpectedHandler()
{
	s_self->setProcessSignaledState();
}

void CrashHandler::pureCallHandler()
{
	s_self->setProcessSignaledState();
}

void CrashHandler::sigAbortHandler(int)
{
	s_self->setProcessSignaledState();
}

void CrashHandler::sigFpeHandler(int code, int subcode)
{
	s_self->setProcessSignaledState();
}

void CrashHandler::sigIntHandler(int)
{
	s_self->setProcessSignaledState();
}

void CrashHandler::sigIllHandler(int)
{
	s_self->setProcessSignaledState();
}

void CrashHandler::sigSegvHandler(int)
{
	s_self->setProcessSignaledState();
}

void CrashHandler::sigTermHandler(int)
{
	s_self->setProcessSignaledState();
}

int CrashHandler::newHandler(size_t)
{
	s_self->setProcessSignaledState();
	return 0;
}

void CrashHandler::invalidParameterHandler(
	const wchar_t* expression,
	const wchar_t* function,
	const wchar_t* file,
	unsigned int line,
	uintptr_t pReserved)
{
	s_self->setProcessSignaledState();
}

bool CrashHandler::lockExceptionFilter()
{
	//
	// We need to patch SetUnhandledExceptionFilter because
	// CRT library calls this function with 0 as a parameter
	// And it clears the already installed SEH handler
	//

	HMODULE kernel32 = GetModuleHandleW(L"KERNEL32.DLL");

	if (kernel32 == nullptr)
	{
		return false;
	}

	void* entryPoint = static_cast<void*>(GetProcAddress(kernel32, "SetUnhandledExceptionFilter"));

	if (entryPoint == nullptr)
	{
		return false;
	}

	DWORD oldProtect = 0;

	if (VirtualProtect(entryPoint, sizeof(s_interceptFunctionOpcodes), PAGE_EXECUTE_READWRITE, &oldProtect) == FALSE)
	{
		return false;
	}

	std::memcpy(entryPoint, s_interceptFunctionOpcodes, sizeof(s_interceptFunctionOpcodes));
	DWORD temp = 0;

	if (VirtualProtect(entryPoint, sizeof(s_interceptFunctionOpcodes), oldProtect, &temp) == FALSE)
	{
		return false;
	}

	return true;
}

}