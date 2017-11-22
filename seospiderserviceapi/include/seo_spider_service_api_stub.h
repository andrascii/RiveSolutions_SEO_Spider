#pragma once

#include "iseo_spider_service_api.h"

namespace SeoSpiderServiceApi
{

class SeoSpiderServiceApiStub : public ISeoSpiderServiceApi
{
public:
    virtual void init() noexcept override
    {
    }
    virtual void free() const noexcept override
    {
    }
    virtual void setProcessSignaledState() const noexcept override
    {
    }
    virtual void setProcessExceptionHandlers() const noexcept override
    {
    }
    virtual void setThreadExceptionHandlers() const noexcept override
    {
    }
    virtual void doAssert(const char*, int, const char*, const char*) const noexcept override
    {
    }
    virtual void debugReport(const char*, int, const char*, const char*) const noexcept override
    {
    }
    virtual void logMessage(const std::string& message) override
    {
    }
};

}