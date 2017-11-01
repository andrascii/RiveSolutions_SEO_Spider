#pragma once

#include "seo_spider_service_api_export.h"

#define ASSERT(condition) if((condition) == false) \
SeoSpiderServiceApi::seoSpiderServiceApi()->doAssert(__FILE__, __LINE__, __FUNCTION__, #condition);

#ifdef QT_DEBUG
#define DEBUG_ASSERT(condition) ASSERT(condition)
#else
#define DEBUG_ASSERT(condition)
#endif

#ifdef QT_DEBUG
#define VERIFY(Connection) ASSERT(Connection)
#else
#define VERIFY(Connection) Connection
#endif