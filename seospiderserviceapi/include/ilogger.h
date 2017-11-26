#pragma once

namespace SeoSpiderServiceApi
{

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

    virtual void setFilter(bool(SeverityLevel level)) noexcept = 0;
    virtual void logMessage(const QString& message, SeverityLevel severityLevel, CallType callType = CallAsync) = 0;
    virtual void flush(CallType callType = CallAsync) = 0;
};

}

Q_DECLARE_METATYPE(SeoSpiderServiceApi::SeverityLevel)