#pragma once

namespace SeoSpiderServiceApi
{

class ILoggerFilter;

enum class SeverityLevel
{
    TraceLevel,
    DebugLevel,
    InfoLevel,
    WarningLevel,
    ErrorLevel
};

class ILogger
{
public:
    enum CallType
    {
        CallAsync,
        CallSync
    };

    // takes ownership of a pointer to filter
    virtual void setFilter(ILoggerFilter* filter) noexcept = 0;

    virtual void logMessage(const QString& message, SeverityLevel severityLevel, CallType callType = CallAsync) = 0;

    virtual void flush(CallType callType = CallAsync) = 0;
};

}