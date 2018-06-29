#pragma once

#include "command.h"

namespace SeoSpiderServiceApi
{

class ISeoSpiderServiceApi
{
public:
	virtual void init() = 0;
	virtual void free() const = 0;
	virtual bool restartApplication(int msec) = 0;
	virtual bool doDump(const void* exceptionInfo, const int exceptionInfoSize) = 0;
	virtual void setProcessExceptionHandlers() const = 0;
	virtual void setThreadExceptionHandlers() const = 0;
	virtual void doAssert(const char* file, int line, const char* function, const char* expression) = 0;
	virtual void setLogFilter(const std::function<bool(Common::LogLevel)>& filter) = 0;

	virtual bool writeLog(
		int id,
		int level,
		const char* file,
		unsigned int line,
		const char* function,
		const void* thisptr,
		const char* text) = 0;
};

}