#include "stdafx.h"
#pragma once

namespace SeoSpiderServiceApi
{
    
class ISeoSpiderServiceApi;
    
}

namespace Common
{

class SeoSpiderServiceApiLoaderPrivate;

class SeoSpiderServiceApiLoader final : public QObject
{
public:
	using SeoSpiderDllMainFunction = SeoSpiderServiceApi::ISeoSpiderServiceApi*(*)();

	static SeoSpiderServiceApiLoader& instance();
	static SeoSpiderServiceApi::ISeoSpiderServiceApi* serviceApi();
	static void disableService();
	static void enableService();

	SeoSpiderDllMainFunction entryPoint();

private:
	SeoSpiderServiceApiLoader();

private:
	static std::atomic_bool m_isDisabled;

	SeoSpiderServiceApiLoaderPrivate* m_impl;
};

}
