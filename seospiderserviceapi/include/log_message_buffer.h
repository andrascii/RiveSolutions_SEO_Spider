#pragma once

#include "pipe_message.h"
#include <sstream>

namespace SeoSpiderServiceApi
{

inline auto getLogFunction(Common::SeverityLevel level)
{
	using LogMemberFunctionType = void(ISeoSpiderServiceApi::*)(
		Common::PipeMessage::Type,
		std::uint64_t,
		std::uint64_t,
		const char*,
		const char*,
		const char*
	);

	using LogFunctionType = void(
		Common::PipeMessage::Type,
		std::uint64_t,
		std::uint64_t,
		const char*,
		const char*,
		const char*
	);

	LogMemberFunctionType memberFunctionPointer = nullptr;

	switch(level)
	{
		case Common::SeverityLevel::TraceLevel:
		{
			memberFunctionPointer = &ISeoSpiderServiceApi::traceLogMessage;
			break;
		}
		case Common::SeverityLevel::DebugLevel:
		{
			memberFunctionPointer = &ISeoSpiderServiceApi::debugLogMessage;
			break;
		}
		case Common::SeverityLevel::InfoLevel:
		{
			memberFunctionPointer = &ISeoSpiderServiceApi::infoLogMessage;
			break;
		}
		case Common::SeverityLevel::WarningLevel:
		{
			memberFunctionPointer = &ISeoSpiderServiceApi::warningLogMessage;
			break;
		}
		case Common::SeverityLevel::ErrorLevel:
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

	return std::function<LogFunctionType>([seoSpiderServiceApiPointer = seoSpiderServiceApi(), memberFunctionPointer](
		Common::PipeMessage::Type type,
		std::uint64_t threadId,
		std::uint64_t line,
		const char* file,
		const char* function,
		const char* message)
	{
		(seoSpiderServiceApiPointer->*memberFunctionPointer)(type, threadId, line, file, function, message);
	});
}

class LogMessageBuffer
{
public:
	LogMessageBuffer(Common::PipeMessage::Type type, Common::SeverityLevel level, 
		std::uint64_t threadId, std::uint64_t line, const char* file, const char* function)
		: m_type(type)
		, m_level(level)
		, m_threadId(threadId)
		, m_line(line)
		, m_file(file)
		, m_function(function)
	{
	}

	~LogMessageBuffer()
	{
		auto&& logFunction = getLogFunction(m_level);

		logFunction(m_type, m_threadId, m_line, m_file, m_function, m_stream.str().c_str());
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
	Common::PipeMessage::Type m_type;
	Common::SeverityLevel m_level;
	std::uint64_t m_threadId;
	std::uint64_t m_line;
	const char* m_file;
	const char* m_function;

	std::stringstream m_stream;
};

}