#pragma once

#include "ilogger.h"

namespace SeoSpiderServiceApi
{

class Logger : public ILogger
{
private:
    template <typename T>
    class DefaultLoggerDeleter;

    template <>
    class DefaultLoggerDeleter<Logger>
    {
    public:
        void operator()(Logger* pointer)
        {
            pointer->m_deleting = true;
            delete pointer;
        }
    };

    friend class DefaultLoggerDeleter<Logger>;

public:
    static ILogger* instance();

    ~Logger();

    virtual void logMessage(const std::string& message, SeverityLevel severityLevel) override;
    virtual void flush() override;

private:
    Logger();

private:
    boost::shared_ptr<boost::log::core> m_loggingCore;

    bool m_deleting;

    static thread_local boost::log::sources::severity_logger<boost::log::trivial::severity_level> s_logger;
};

}