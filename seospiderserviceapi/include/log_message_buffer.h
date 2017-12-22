#pragma once

#include "ilogger.h"
#include <sstream>
#include <iostream>

namespace SeoSpiderServiceApi
{

inline std::function<void(const char*)> getLogFunction(SeverityLevel level)
{
    using LogMemberFunctionType = void(ISeoSpiderServiceApi::*)(const char*);

    LogMemberFunctionType memberFunctionPointer = nullptr;

    switch(level)
    {
        case SeverityLevel::TraceLevel:
        {
            memberFunctionPointer = &ISeoSpiderServiceApi::traceLogMessage;
            break;
        }
        case SeverityLevel::DebugLevel:
        {
            memberFunctionPointer = &ISeoSpiderServiceApi::debugLogMessage;
            break;
        }
        case SeverityLevel::InfoLevel:
        {
            memberFunctionPointer = &ISeoSpiderServiceApi::infoLogMessage;
            break;
        }
        case SeverityLevel::WarningLevel:
        {
            memberFunctionPointer = &ISeoSpiderServiceApi::warningLogMessage;
            break;
        }
        case SeverityLevel::ErrorLevel:
        {
            memberFunctionPointer = &ISeoSpiderServiceApi::errorLogMessage;
            break;
        }
        default:
        {
        #ifndef PRODUCTION
            abort();
        #endif
        }
    }

    return std::function<void(const char*)>([seoSpiderServiceApiPointer = seoSpiderServiceApi(), memberFunctionPointer](const char* s)
    {
        (seoSpiderServiceApiPointer->*memberFunctionPointer)(s);
    });
}

class LogMessageBuffer
{
public:
    LogMessageBuffer(SeverityLevel level)
        : m_level(level)
    {
    }

    ~LogMessageBuffer()
    {
        auto&& logFunction = getLogFunction(m_level);

        logFunction(m_stream.str().c_str());
    }

    template <typename T>
    LogMessageBuffer& operator<<(const T& t)
    {
        m_stream << t << " ";

        return *this;
    }

    template <>
    LogMessageBuffer& operator<< <QString>(const QString& s)
    {
        return *this << s.toStdString();
    }

    template <>
    LogMessageBuffer& operator<< <QByteArray>(const QByteArray& ba)
    {
        return *this << ba.data();
    }

private:
    SeverityLevel m_level;

    std::stringstream m_stream;
};

}