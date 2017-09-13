#define EXPORT

#include "iwatch_dog_service_api.h"
#include "watch_dog_service_api.h"
#include "watch_dog_service_api_stub.h"
#include "watch_dog_api.h"

WatchDog::IWatchDogServiceApi* watchDogServiceApi()
{
	static std::unique_ptr<WatchDog::IWatchDogServiceApi> s_serviceApi;

	if (s_serviceApi)
	{
		return s_serviceApi.get();
	}

	s_serviceApi = std::make_unique<WatchDog::WatchDogServiceApi>();

	return s_serviceApi.get();
}

static const BYTE s_patchBytes[] =
{
#ifdef _WIN64
	//	xor rax,rax
	//	ret
	0x48, 0x31, 0xC0, 0xC3
#else
	//	xor eax,eax
	//	ret 4
	0x33, 0xC0, 0xC2, 0x04, 0x00
#endif
};

bool lockExceptionFilter()
{
	//
	// We need to patch SetUnhandledExceptionFilter because
	// CRT library calls this function with 0 as a parameter
	// And it clears the already installed SEH handler
	//

	HMODULE kernel32 = GetModuleHandleW(L"KERNEL32.DLL");
	if (kernel32 == NULL)
	{
		return false;
	}

	BYTE* entryPoint = (BYTE*)GetProcAddress(kernel32, "SetUnhandledExceptionFilter");
	if (entryPoint == NULL)
	{
		return false;
	}

	DWORD oldProtect = 0;
	if (VirtualProtect(entryPoint, sizeof(s_patchBytes), PAGE_EXECUTE_READWRITE, &oldProtect) == FALSE)
	{
		return false;
	}

	memcpy(entryPoint, s_patchBytes, sizeof(s_patchBytes));
	DWORD temp = 0;

	if (VirtualProtect(entryPoint, sizeof(s_patchBytes), oldProtect, &temp) == FALSE)
	{
		return false;
	}

	return true;
}

extern "C" WATCH_DOG_API_EXPORT void initializeWatchDog()
{
	watchDogServiceApi()->setProcessExceptionHandlers();
	watchDogServiceApi()->setThreadExceptionHandlers();

	lockExceptionFilter();
}

BOOL WINAPI DllMain(HINSTANCE hinstDLL, DWORD fdwReason, LPVOID lpvReserved)
{
	switch (fdwReason)
	{
		case DLL_PROCESS_ATTACH:
		{
			watchDogServiceApi()->setProcessExceptionHandlers();
			break;
		}
		case DLL_THREAD_ATTACH:
		{
			watchDogServiceApi()->setThreadExceptionHandlers();
			break;
		}
	}

	return TRUE;
}