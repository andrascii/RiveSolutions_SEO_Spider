#include "ipc_signaled_object_windows.h"

namespace Common
{

IpcSignaledObjectWindows::IpcSignaledObjectWindows()
	: m_eventHandle()
	, m_isNull(true)
{
}

IpcSignaledObjectWindows::~IpcSignaledObjectWindows()
{
	CloseHandle(m_eventHandle);
}

bool IpcSignaledObjectWindows::create(const std::wstring& name) noexcept
{
	SECURITY_ATTRIBUTES securityAttributes{ 0 };

	m_eventHandle = CreateEventW(&securityAttributes, FALSE, FALSE, name.c_str());

	if (GetLastError() == ERROR_ALREADY_EXISTS)
	{
		CloseHandle(m_eventHandle);
		return false;
	}

	m_isNull = false;

	return true;
}

bool IpcSignaledObjectWindows::open(const std::wstring& name) noexcept
{
	m_eventHandle = OpenEventW(SYNCHRONIZE, FALSE, name.c_str());

	if (m_eventHandle == NULL)
	{
		return false;
	}

	m_isNull = false;

	return true;
}

bool IpcSignaledObjectWindows::isNull() const noexcept
{
	return m_isNull;
}

void IpcSignaledObjectWindows::setSignaledState() noexcept
{
	if (m_isNull)
	{
		abort();
	}

	SetEvent(m_eventHandle);
}

void IpcSignaledObjectWindows::resetState() noexcept
{
	if (m_isNull)
	{
		abort();
	}

	ResetEvent(m_eventHandle);
}

IIpcSignaledObject::WakeupReason IpcSignaledObjectWindows::waitFor(std::uint32_t milliseconds) noexcept
{
	DWORD result = WaitForSingleObject(m_eventHandle, milliseconds);

	switch (result)
	{
		case WAIT_OBJECT_0:
		{
			return WakeupReason::ObjectSetSignaledStateReason;
		}
		case WAIT_TIMEOUT:
		{
			return WakeupReason::TimeoutReason;
		}
		case WAIT_FAILED:
		{
			return WakeupReason::WaitFailedBySystemReason;
		}
		default:
		{
			return WakeupReason::UnknownErrorReason;
		}
	}
}

}