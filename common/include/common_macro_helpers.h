#pragma once

#include "seo_spider_service_api_export.h"
#include "log_message_buffer.h"

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

#define INFOLOG  SeoSpiderServiceApi::LogMessageBuffer(SeoSpiderServiceApi::ILogger::InfoLevel)
#define DEBUGLOG SeoSpiderServiceApi::LogMessageBuffer(SeoSpiderServiceApi::ILogger::DebugLevel)
#define WARNLOG  SeoSpiderServiceApi::LogMessageBuffer(SeoSpiderServiceApi::ILogger::WarningLevel)
#define ERRLOG   SeoSpiderServiceApi::LogMessageBuffer(SeoSpiderServiceApi::ILogger::ErrorLevel)