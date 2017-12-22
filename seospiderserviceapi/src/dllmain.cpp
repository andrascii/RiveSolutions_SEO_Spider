#include "seo_spider_service_api_impl.h"
#include "seo_spider_service_api_stub.h"

extern "C" Q_DECL_EXPORT SeoSpiderServiceApi::ISeoSpiderServiceApi* seoSpiderServiceApi()
{
	static std::unique_ptr<SeoSpiderServiceApi::ISeoSpiderServiceApi> s_serviceApi;

	if (s_serviceApi)
	{
		return s_serviceApi.get();
	}

#ifdef Q_OS_WIN
	s_serviceApi = std::make_unique<SeoSpiderServiceApi::SeoSpiderServiceApiImpl>();
#else
	s_serviceApi = std::make_unique<SeoSpiderServiceApi::SeoSpiderServiceApiStub>();
#endif

	return s_serviceApi.get();
}

BOOL WINAPI DllMain(HINSTANCE hinstDLL, DWORD fdwReason, LPVOID lpvReserved)
{
	Q_UNUSED(hinstDLL);
	Q_UNUSED(lpvReserved);

	switch (fdwReason)
	{
		case DLL_PROCESS_ATTACH:
		{
			seoSpiderServiceApi()->init();
			seoSpiderServiceApi()->setProcessExceptionHandlers();
			seoSpiderServiceApi()->setThreadExceptionHandlers();

			break;
		}
		case DLL_THREAD_ATTACH:
		{
			seoSpiderServiceApi()->setThreadExceptionHandlers();

			break;
		}
		case DLL_PROCESS_DETACH:
		{
			seoSpiderServiceApi()->free();
		}
	}

	return TRUE;
}