#include "logger.h"
#include "default_logger_filter.h"
#include "iseo_spider_service_api.h"

namespace SeoSpiderServiceApi
{

SeoSpiderServiceApi::ILogger* Logger::instance()
{
    static std::unique_ptr<Logger, DefaultLoggerDeleter<Logger>> s_instance;

    if (!s_instance)
    {
        s_instance.reset(new Logger);
    }

    return s_instance.get();
}

Logger::~Logger()
{
    if (!m_deleting)
    {
        // this already deleted
        abort();
    }
}

void Logger::setFilter(ILoggerFilter* filter) noexcept
{
    m_filter.reset(filter);
}

void Logger::logMessage(const QString& message, SeverityLevel level, ILogger::CallType callType)
{
    Qt::ConnectionType connectionType = callType == CallAsync ? Qt::QueuedConnection : Qt::BlockingQueuedConnection;

    QMetaObject::invokeMethod(&m_logWriterThread, "logMessage", connectionType, Q_ARG(const QString&, message));
}

void Logger::flush(ILogger::CallType callType)
{
    Qt::ConnectionType connectionType = callType == CallAsync ? Qt::QueuedConnection : Qt::BlockingQueuedConnection;

    QMetaObject::invokeMethod(&m_logWriterThread, "flush", connectionType);
}

Logger::Logger()
    : m_deleting(false)
{
    setFilter(new DefaultLoggerFilter);
}

bool Logger::messageTypeAvailable(SeverityLevel level) const noexcept
{
    return m_filter ? (*m_filter)(level) : true;
}

}