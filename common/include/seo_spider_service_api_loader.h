#pragma once

#include "iseo_spider_service_api.h"

namespace Common
{

class SeoSpiderServiceApiLoaderPrivate;

class SeoSpiderServiceApiLoader final : public QObject
{
public:
	using SeoSpiderDllMainFunction = SeoSpiderServiceApi::ISeoSpiderServiceApi*(*)();

	static SeoSpiderServiceApiLoader& instance();
	static SeoSpiderServiceApi::ISeoSpiderServiceApi* serviceApi();

	SeoSpiderDllMainFunction entryPoint();

private:
	SeoSpiderServiceApiLoader();

private:
	SeoSpiderServiceApiLoaderPrivate* m_impl;
};

}