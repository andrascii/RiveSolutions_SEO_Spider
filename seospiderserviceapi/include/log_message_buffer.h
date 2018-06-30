#pragma once

#include "command.h"
#include <sstream>

namespace SeoSpiderServiceApi
{

class LogMessageBuffer
{
public:
	LogMessageBuffer(Common::LogLevel level, std::uint64_t line, const char* file, const char* function)
		: m_level(level)
		, m_line(line)
		, m_file(file)
		, m_function(function)
	{
	}

	~LogMessageBuffer()
	{
		seoSpiderServiceApi()->writeLog(0, m_level, m_file, m_line, m_function, this, m_stream.str().c_str());
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
	Common::LogLevel m_level;
	std::uint64_t m_line;
	const char* m_file;
	const char* m_function;

	std::stringstream m_stream;
};

}