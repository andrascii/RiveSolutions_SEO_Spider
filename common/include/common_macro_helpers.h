#pragma once

#include "seo_spider_service_api_export.h"
#include "log_message_buffer.h"

#ifdef Q_OS_WIN
#define __FILENAME__ (strrchr(__FILE__, '\\') ? strrchr(__FILE__, '\\') + 1 : __FILE__)
#else
#define __FILENAME__ (strrchr(__FILE__, '/') ? strrchr(__FILE__, '/') + 1 : __FILE__)
#endif

#define ASSERT(condition) if((condition) == false) \
seoSpiderServiceApi()->doAssert(__FILENAME__, __LINE__, __FUNCTION__, #condition);

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

#define TRACELOG SeoSpiderServiceApi::LogMessageBuffer(Common::PipeMessage::LogMessage, Common::SeverityLevel::TraceLevel, std::hash<std::thread::id>{}(std::this_thread::get_id()), __LINE__, __FILENAME__, __FUNCTION__)
#define DEBUGLOG SeoSpiderServiceApi::LogMessageBuffer(Common::PipeMessage::LogMessage, Common::SeverityLevel::DebugLevel, std::hash<std::thread::id>{}(std::this_thread::get_id()), __LINE__, __FILENAME__, __FUNCTION__)
#define INFOLOG  SeoSpiderServiceApi::LogMessageBuffer(Common::PipeMessage::LogMessage, Common::SeverityLevel::InfoLevel, std::hash<std::thread::id>{}(std::this_thread::get_id()), __LINE__, __FILENAME__, __FUNCTION__)
#define WARNLOG  SeoSpiderServiceApi::LogMessageBuffer(Common::PipeMessage::LogMessage, Common::SeverityLevel::WarningLevel, std::hash<std::thread::id>{}(std::this_thread::get_id()), __LINE__, __FILENAME__, __FUNCTION__)
#define ERRLOG   SeoSpiderServiceApi::LogMessageBuffer(Common::PipeMessage::LogMessage, Common::SeverityLevel::ErrorLevel, std::hash<std::thread::id>{}(std::this_thread::get_id()), __LINE__, __FILENAME__, __FUNCTION__)