#include "iwatch_dog_service_api.h"
#include "watch_dog_service_api.h"
#include "watch_dog_service_api_stub.h"

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

__declspec(dllexport) void test()
{
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