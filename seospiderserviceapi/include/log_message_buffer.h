#pragma once

#include "ilogger.h"
#include <sstream>
#include <iostream>

namespace SeoSpiderServiceApi
{

class LogMessageBuffer
{
public:
    LogMessageBuffer(SeverityLevel level)
        : m_level(level)
    {
    }

    ~LogMessageBuffer()
    {
        seoSpiderServiceApi()->logMessage(m_stream.str().c_str(), m_level);
    }

    template <typename T>
    LogMessageBuffer& operator<<(const T& t)
    {
        m_stream << t;

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