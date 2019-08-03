#pragma once
#include "stdafx.h"

#include "iseo_spider_service_api.h"

namespace SeoSpiderServiceApi
{

class SeoSpiderServiceApiStub : public ISeoSpiderServiceApi
{
public:
	virtual void init() override
	{
	}
	virtual void free() const override
	{
	}
	virtual bool restartApplication(int) override
	{
		return false;
	}
	virtual bool doDump(const void*, const int) override
	{
		return false;
	}
	virtual void setProcessExceptionHandlers() const override
	{
	}
	virtual void setThreadExceptionHandlers() const override
	{
	}
	virtual void doAssert(const char*, int, const char*, const char*, const char*) override
	{
	}
	virtual void setLogFilter(const std::function<bool(Common::LogLevel)>&) override
	{
	}
	virtual void applicationInitialized(const char*, const char*, const char*, const char*, const char*, const char*) override
	{
	}
	virtual void commitCounterData(const char*, quint64, int) override
	{
	}
	virtual bool writeLog(int, int, const char*, unsigned int, const char*, const void*, const char*) override
	{
		return false;
	}
};

}