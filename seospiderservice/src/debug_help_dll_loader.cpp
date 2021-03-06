#include "debug_help_dll_loader.h"

namespace SeoSpiderService
{

DebugHelpDllLoader::DebugHelpDllLoader()
{
	load();
}

DebugHelpDllLoader::~DebugHelpDllLoader()
{
	if (isLoaded())
	{
		FreeLibrary(m_module);
	}
}

void DebugHelpDllLoader::load() noexcept
{
	m_module = LoadLibraryW(L"dbghelp.dll");

	if (!m_module)
	{
		qDebug() << "Can't load dbghelp.dll";
	}

	m_writeDumpFunction = reinterpret_cast<WriteDumpFunctionType>(GetProcAddress(m_module, "MiniDumpWriteDump"));

	if (!m_writeDumpFunction)
	{
		qDebug() << "Can't extract MiniDumpWriteDump function address from dll";
	}
}

bool DebugHelpDllLoader::isLoaded() const noexcept
{
	return m_module;
}

bool DebugHelpDllLoader::writeDump(
	HANDLE processHandle, 
	DWORD processId, 
	HANDLE dumpFileHandle, 
	MINIDUMP_TYPE miniDumpType, 
	PMINIDUMP_EXCEPTION_INFORMATION exceptionInformation, 
	PMINIDUMP_USER_STREAM_INFORMATION userStreamInformation, 
	PMINIDUMP_CALLBACK_INFORMATION callbackInformation) const noexcept
{
	Q_ASSERT(isLoaded() && "Attempt to write dump with unloaded dbghelp.dll");

	if (!isLoaded())
	{
		qDebug() << "Attempt to write dump with unloaded dbghelp.dll";
		return false;
	}

	BOOL result = m_writeDumpFunction(processHandle, processId, dumpFileHandle, miniDumpType, exceptionInformation, userStreamInformation, callbackInformation);

	if (!result)
	{
		qDebug() << "Can't write minidump!";
	}

	return static_cast<bool>(result);
}

}