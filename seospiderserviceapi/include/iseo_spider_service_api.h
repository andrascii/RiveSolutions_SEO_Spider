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
    virtual void debugReport(const char* file, int line, const char* function, const char* expression) const noexcept = 0;

    virtual void logMessage(const std::string& message) = 0;
};

}