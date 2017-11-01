#include "crash_handler.h"
#include "crash_handler_stub.h"

namespace SeoSpiderServiceApi
{

ICrashHandler* crashHandler()
{
	static std::unique_ptr<SeoSpiderServiceApi::ICrashHandler> s_serviceApi;

	if (s_serviceApi)
	{
		return s_serviceApi.get();
	}

#ifdef Q_OS_WIN
	s_serviceApi = std::make_unique<SeoSpiderServiceApi::CrashHandler>();
#else
	s_serviceApi = std::make_unique<SeoSpiderServiceApi::CrashHandlerStub>();
#endif

	return s_serviceApi.get();
}

}

BOOL WINAPI DllMain(HINSTANCE hinstDLL, DWORD fdwReason, LPVOID lpvReserved)
{
	Q_UNUSED(hinstDLL);
	Q_UNUSED(lpvReserved);

	switch (fdwReason)
	{
		case DLL_PROCESS_ATTACH:
		{
			SeoSpiderServiceApi::crashHandler()->init();
			SeoSpiderServiceApi::crashHandler()->setProcessExceptionHandlers();

			break;
		}
		case DLL_THREAD_ATTACH:
		{
			SeoSpiderServiceApi::crashHandler()->setThreadExceptionHandlers();
			break;
		}
		case DLL_PROCESS_DETACH:
		{
			SeoSpiderServiceApi::crashHandler()->free();
		}
	}

	return TRUE;
}