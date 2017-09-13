#include "iwatch_dog_service_api.h"
#include "watch_dog_api_private.h"

BOOL WINAPI DllMain(HINSTANCE hinstDLL, DWORD fdwReason, LPVOID lpvReserved)
{
	switch (fdwReason)
	{
		case DLL_PROCESS_ATTACH:
		{
			WatchDogApi::watchDogServiceApi()->setProcessExceptionHandlers();

			WatchDogApi::lockExceptionFilter();

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