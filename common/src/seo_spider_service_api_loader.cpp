#include "seo_spider_service_api_loader.h"
#include "seo_spider_service_api_stub.h"

namespace Common
{

class SeoSpiderServiceApiLoaderPrivate final : public QObject
{
public:
	SeoSpiderServiceApiLoaderPrivate(QObject* parent)
		: QObject(parent)
		, m_library(new QLibrary("seospiderserviceapi.dll", this))
		, m_functionPointer(nullptr)
	{
	}

	QFunctionPointer entryPoint()
	{
		if (!m_functionPointer)
		{
			m_functionPointer = m_library->resolve("seoSpiderServiceApi");
		}

		return m_functionPointer;
	}

private:
	QLibrary* m_library;
	QFunctionPointer m_functionPointer;
};

SeoSpiderServiceApiLoader&
SeoSpiderServiceApiLoader::instance()
{
	static std::mutex s_mutex;
	static std::unique_ptr<SeoSpiderServiceApiLoader> s_instance;

	std::lock_guard locker(s_mutex);

	if (!s_instance)
	{
		s_instance.reset(new SeoSpiderServiceApiLoader);
	}

	return *s_instance.get();
}

SeoSpiderServiceApi::ISeoSpiderServiceApi* SeoSpiderServiceApiLoader::serviceApi()
{
	static std::unique_ptr<SeoSpiderServiceApi::ISeoSpiderServiceApi>
		s_serviceApiStub(new SeoSpiderServiceApi::SeoSpiderServiceApiStub);

	SeoSpiderServiceApiLoader& loader = instance();

	if (!loader.entryPoint())
	{
		return s_serviceApiStub.get();
	}

	return loader.entryPoint()();
}

SeoSpiderServiceApiLoader::SeoSpiderServiceApiLoader()
	: QObject(nullptr)
	, m_impl(new SeoSpiderServiceApiLoaderPrivate(this))
{
}

SeoSpiderServiceApiLoader::SeoSpiderDllMainFunction
SeoSpiderServiceApiLoader::entryPoint()
{
	return reinterpret_cast<SeoSpiderDllMainFunction>(m_impl->entryPoint());
}

}