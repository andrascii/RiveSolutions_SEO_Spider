#pragma once

#include "ilogger.h"
#include "ilogger_filter.h"
#include "log_writer.h"

namespace SeoSpiderServiceApi
{

class Logger : public QObject, public ILogger
{
	Q_OBJECT

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

public:
	static ILogger* instance();

	~Logger();

	virtual void setFilter(bool(f)(SeverityLevel level)) noexcept override;

	virtual void logMessage(const QString& message, SeverityLevel level, CallType callType = CallAsync) override;

	virtual void flush(CallType callType = CallAsync) override;

private:
	Logger();

	bool messageTypeAvailable(SeverityLevel level) const noexcept;

private:
	bool m_deleting;

	LogWriter* m_logWriter;

	std::unique_ptr<ILoggerFilter> m_filter;
};

}