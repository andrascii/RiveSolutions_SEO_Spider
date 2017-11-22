#pragma once

#include "ilogger.h"

namespace SeoSpiderServiceApi
{

class Logger : public ILogger
{
public:
    static ILogger* instance();

    virtual void logMessage(const std::string& message, SeverityLevel severityLevel) override;

private:
    Logger();

private:
    boost::shared_ptr<boost::log::core> m_loggingCore;

    static thread_local boost::log::sources::severity_logger<boost::log::trivial::severity_level> s_logger;
};

}