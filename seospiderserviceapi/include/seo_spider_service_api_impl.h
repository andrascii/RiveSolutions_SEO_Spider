#pragma once

#include "iseo_spider_service_api.h"

namespace SeoSpiderServiceApi
{

class SeoSpiderServiceApiImpl : public ISeoSpiderServiceApi
{
public:
    SeoSpiderServiceApiImpl();

    virtual void init() noexcept override;
    virtual void free() const noexcept override;
    
    virtual void setProcessSignaledState() const noexcept override;

    virtual void setProcessExceptionHandlers() const noexcept override;
    virtual void setThreadExceptionHandlers() const noexcept override;

    virtual void doAssert(const char* file, int line, const char* function, const char* expression) const noexcept override;

    virtual void traceLogMessage(const char* message) override;
    virtual void debugLogMessage(const char* message) override;
    virtual void infoLogMessage(const char* message) override;
    virtual void warningLogMessage(const char* message) override;
    virtual void errorLogMessage(const char* message) override;

private:
    void debugReport(const char* file, int line, const char* function, const char* expression) const noexcept;

    static LONG WINAPI sehHandler(PEXCEPTION_POINTERS pExceptionPtrs);
    static void pureCallHandler();
    static void terminateHandler();
    static void unexpectedHandler();
    static void sigAbortHandler(int);
    static void sigFpeHandler(int code, int subcode);
    static void sigIntHandler(int);
    static void sigIllHandler(int);
    static void sigSegvHandler(int);
    static void sigTermHandler(int);
    static int newHandler(size_t);

    static void invalidParameterHandler(
        const wchar_t* expression,
        const wchar_t* function,
        const wchar_t* file,
        unsigned int line, uintptr_t pReserved);
    
    static bool lockExceptionFilter();

private:
    static SeoSpiderServiceApiImpl* s_self;

    HANDLE m_crashEventHandle;

    mutable std::mutex m_mutex;

    bool m_initialized;
};

}