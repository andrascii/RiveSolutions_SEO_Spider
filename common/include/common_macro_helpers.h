#pragma once

#include "seo_spider_service_api_export.h"
#include "log_message_buffer.h"

#define ASSERT(condition) if((condition) == false) \
seoSpiderServiceApi()->doAssert(__FILE__, __LINE__, __FUNCTION__, #condition);

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

#define TRACELOG SeoSpiderServiceApi::LogMessageBuffer(SeoSpiderServiceApi::SeverityLevel::TraceLevel)   << __FILENAME__ << "|" << __LINE__ << "|" << __FUNCTION__ << "| thread_id(" << std::this_thread::get_id() << ")"
#define DEBUGLOG SeoSpiderServiceApi::LogMessageBuffer(SeoSpiderServiceApi::SeverityLevel::DebugLevel)   << __FILENAME__ << "|" << __LINE__ << "|" << __FUNCTION__ << "| thread_id(" << std::this_thread::get_id() << ")"
#define INFOLOG  SeoSpiderServiceApi::LogMessageBuffer(SeoSpiderServiceApi::SeverityLevel::InfoLevel)    << __FILENAME__ << "|" << __LINE__ << "|" << __FUNCTION__ << "| thread_id(" << std::this_thread::get_id() << ")"
#define WARNLOG  SeoSpiderServiceApi::LogMessageBuffer(SeoSpiderServiceApi::SeverityLevel::WarningLevel) << __FILENAME__ << "|" << __LINE__ << "|" << __FUNCTION__ << "| thread_id(" << std::this_thread::get_id() << ")"
#define ERRLOG   SeoSpiderServiceApi::LogMessageBuffer(SeoSpiderServiceApi::SeverityLevel::ErrorLevel)   << __FILENAME__ << "|" << __LINE__ << "|" << __FUNCTION__ << "| thread_id(" << std::this_thread::get_id() << ")"