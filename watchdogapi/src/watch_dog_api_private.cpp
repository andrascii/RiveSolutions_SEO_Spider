#define EXPORT

#include "watch_dog_api.h"
#include "watch_dog_api_private.h"
#include "iwatch_dog_service_api.h"
#include "watch_dog_service_api.h"
#include "watch_dog_service_api_stub.h"

namespace
{

static const BYTE s_patchBytes[] =
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

namespace WatchDogApi
{

IWatchDogServiceApi* watchDogServiceApi()
{
	static std::unique_ptr<WatchDogApi::IWatchDogServiceApi> s_serviceApi;

	if (s_serviceApi)
	{
		return s_serviceApi.get();
	}

#ifdef Q_OS_WIN
	s_serviceApi = std::make_unique<WatchDogApi::WatchDogServiceApi>();
#else
	s_serviceApi = std::make_unique<WatchDogApi::WatchDogServiceApiStub>();
#endif

	return s_serviceApi.get();
}

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

	void* entryPoint = static_cast<void*>(GetProcAddress(kernel32, "SetUnhandledExceptionFilter"));

	if (entryPoint == NULL)
	{
		return false;
	}

	DWORD oldProtect = 0;
	if (VirtualProtect(entryPoint, sizeof(s_patchBytes), PAGE_EXECUTE_READWRITE, &oldProtect) == FALSE)
	{
		return false;
	}

	std::memcpy(entryPoint, s_patchBytes, sizeof(s_patchBytes));
	DWORD temp = 0;

	if (VirtualProtect(entryPoint, sizeof(s_patchBytes), oldProtect, &temp) == FALSE)
	{
		return false;
	}

	return true;
}

}

extern "C" WATCH_DOG_API_EXPORT void initWatchDogApi()
{
	WatchDogApi::watchDogServiceApi()->setThreadExceptionHandlers();
}