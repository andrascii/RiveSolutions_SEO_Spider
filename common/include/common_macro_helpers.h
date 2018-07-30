#pragma once

#include "seo_spider_service_api_loader.h"
#include "log_message_buffer.h"

#ifdef Q_OS_WIN
#define __FILENAME__ (strrchr(__FILE__, '\\') ? strrchr(__FILE__, '\\') + 1 : __FILE__)
#else
#define __FILENAME__ (strrchr(__FILE__, '/') ? strrchr(__FILE__, '/') + 1 : __FILE__)
#endif

#define ASSERT(condition) \
if((condition) == false) \
{\
	Common::SeoSpiderServiceApiLoader::serviceApi()->doAssert(__FILENAME__, __LINE__, __FUNCTION__, #condition); \
} 0

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

#define DEBUGLOG SeoSpiderServiceApi::LogMessageBuffer(Common::LogLevel::DebugLog, __LINE__, __FILENAME__, __FUNCTION__)
#define INFOLOG  SeoSpiderServiceApi::LogMessageBuffer(Common::LogLevel::InfoLog, __LINE__, __FILENAME__, __FUNCTION__)
#define WARNLOG  SeoSpiderServiceApi::LogMessageBuffer(Common::LogLevel::WarningLog, __LINE__, __FILENAME__, __FUNCTION__)
#define ERRLOG   SeoSpiderServiceApi::LogMessageBuffer(Common::LogLevel::ErrorLog, __LINE__, __FILENAME__, __FUNCTION__)