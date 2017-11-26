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


void qtMsgHandler(QtMsgType type, const QMessageLogContext&, const QString& msg)
{
    switch (type)
    {
    case QtDebugMsg:
        SeoSpiderServiceApi::Logger::instance()->logMessage(msg, SeoSpiderServiceApi::SeverityLevel::DebugLevel);
        break;

    case QtWarningMsg:
        SeoSpiderServiceApi::Logger::instance()->logMessage(msg, SeoSpiderServiceApi::SeverityLevel::WarningLevel);
        break;

    case QtCriticalMsg:
        SeoSpiderServiceApi::Logger::instance()->logMessage(msg, SeoSpiderServiceApi::SeverityLevel::ErrorLevel);
        break;

    case QtFatalMsg:
        SeoSpiderServiceApi::Logger::instance()->logMessage(msg, SeoSpiderServiceApi::SeverityLevel::ErrorLevel);
        abort();
        break;
    }
}

}

namespace SeoSpiderServiceApi
{

SeoSpiderServiceApi::ILogger* Logger::instance()
{
    static std::mutex s_mutex;
    static std::unique_ptr<Logger, DefaultLoggerDeleter<Logger>> s_instance;

    std::lock_guard<std::mutex> locker(s_mutex);

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

    QString messageWithTimeStamp = QDateTime::currentDateTime().toString(Qt::RFC2822Date) + "|" + message;

    Qt::ConnectionType connectionType = callType == CallAsync ? Qt::QueuedConnection : Qt::BlockingQueuedConnection;

    QMetaObject::invokeMethod(&m_logWriterThread, "logMessage", connectionType,
        Q_ARG(const QString&, messageWithTimeStamp), Q_ARG(SeverityLevel, level));
}

void Logger::flush(ILogger::CallType callType)
{
    Qt::ConnectionType connectionType = callType == CallAsync ? Qt::QueuedConnection : Qt::BlockingQueuedConnection;

    QMetaObject::invokeMethod(&m_logWriterThread, "flush", connectionType);
}

Logger::Logger()
    : m_deleting(false)
{
    qInstallMessageHandler(qtMsgHandler);

    m_filter.reset(new DefaultLoggerFilter);
}

bool Logger::messageTypeAvailable(SeverityLevel level) const noexcept
{
    return m_filter ? (*m_filter)(level) : true;
}

}