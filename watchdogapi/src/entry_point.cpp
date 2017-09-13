#include "iwatch_dog_service_api.h"
#include "watch_dog_api_private.h"

BOOL WINAPI DllMain(HINSTANCE hinstDLL, DWORD fdwReason, LPVOID lpvReserved)
{
	Q_UNUSED(hinstDLL);
	Q_UNUSED(lpvReserved);

	switch (fdwReason)
	{
		case DLL_PROCESS_ATTACH:
		{
			WatchDogApi::watchDogServiceApi()->setProcessExceptionHandlers();

			WatchDogApi::lockExceptionFilter();

			CreateEvent(nullptr, FALSE, FALSE, "WatchDogApiCrashEvent");

			break;
		}
		case DLL_THREAD_ATTACH:
		{
			WatchDogApi::watchDogServiceApi()->setThreadExceptionHandlers();
			break;
		}
	}

	return TRUE;
}