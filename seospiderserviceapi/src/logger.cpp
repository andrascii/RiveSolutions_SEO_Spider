#include "logger.h"
#include "default_logger_filter.h"
#include "iseo_spider_service_api.h"

namespace
{

template <typename F>
class CustomLoggerFilter : public SeoSpiderServiceApi::ILoggerFilter
{
public:
    CustomLoggerFilter(F f)
        : m_f(f)
    {
    }

    virtual bool operator()(SeoSpiderServiceApi::SeverityLevel level) const noexcept override
    {
        return m_f(level);
    }

private:
    std::decay_t<F> m_f;
};

}

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

void Logger::setFilter(bool(f)(SeverityLevel level)) noexcept
{
    m_filter.reset(new CustomLoggerFilter<bool(SeverityLevel level)>(f));
}

void Logger::logMessage(const QString& message, SeverityLevel level, ILogger::CallType callType)
{
    if (!messageTypeAvailable(level))
    {
        return;
    }

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
    m_filter.reset(new DefaultLoggerFilter);
}

bool Logger::messageTypeAvailable(SeverityLevel level) const noexcept
{
    return m_filter ? (*m_filter)(level) : true;
}

}