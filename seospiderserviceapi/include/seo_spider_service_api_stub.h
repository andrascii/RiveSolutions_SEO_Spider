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
    virtual void traceLogMessage(const char*) override
    {
    }
    virtual void debugLogMessage(const char*) override
    {
    }
    virtual void infoLogMessage(const char*) override
    {
    }
    virtual void warningLogMessage(const char*) override
    {
    }
    virtual void errorLogMessage(const char*) override
    {
    }
};

}