#pragma once

namespace SeoSpiderServiceApi
{

class ISeoSpiderServiceApi
{
public:
    virtual void init() noexcept = 0;
    virtual void free() const noexcept = 0;
    
    virtual void setProcessSignaledState() const noexcept = 0;
    virtual void setProcessExceptionHandlers() const noexcept = 0;
    virtual void setThreadExceptionHandlers() const noexcept = 0;

    virtual void doAssert(const char* file, int line, const char* function, const char* expression) const noexcept = 0;

    virtual void traceLogMessage(const char* message) = 0;
    virtual void debugLogMessage(const char* message) = 0;
    virtual void infoLogMessage(const char* message) = 0;
    virtual void warningLogMessage(const char* message) = 0;
    virtual void errorLogMessage(const char* message) = 0;
};

}