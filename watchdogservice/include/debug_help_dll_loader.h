#pragma once

namespace WatchDogService
{

class DebugHelpDllLoader final
{
public:
	DebugHelpDllLoader();
	~DebugHelpDllLoader();

	void writeDump(
		HANDLE,
		DWORD,
		HANDLE,
		MINIDUMP_TYPE,
		PMINIDUMP_EXCEPTION_INFORMATION,
		PMINIDUMP_USER_STREAM_INFORMATION,
		PMINIDUMP_CALLBACK_INFORMATION) const noexcept;

private:
	void load() noexcept;
	bool isLoaded() const noexcept;

private:
	typedef BOOL(WINAPI *WriteDumpFunctionType)(
		HANDLE,
		DWORD,
		HANDLE, 
		MINIDUMP_TYPE, 
		PMINIDUMP_EXCEPTION_INFORMATION,
		PMINIDUMP_USER_STREAM_INFORMATION,
		PMINIDUMP_CALLBACK_INFORMATION
		);

	WriteDumpFunctionType m_writeDumpFunction;
	HMODULE m_module;
};

}