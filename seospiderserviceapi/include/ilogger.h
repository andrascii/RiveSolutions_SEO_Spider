#pragma once

namespace SeoSpiderServiceApi
{

class ILogger
{
public:
	enum SeverityLevel
	{
		TraceLevel,
		DebugLevel,
		InfoLevel,
		WarningLevel,
		ErrorLevel
	};

	virtual void logMessage(const std::string& message, SeverityLevel severityLevel) = 0;
};

}