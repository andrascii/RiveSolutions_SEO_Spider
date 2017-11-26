#include "seo_spider_service_api_impl.h"
#include "logger.h"

namespace
{

const std::uint8_t s_interceptFunctionOpcodes[] =
{
#ifdef _WIN64
    //  XOR RAX, RAX
    //  RET
    0x48, 0x31, 0xC0, 0xC3
#else
    //  XOR EAX, EAX
    //  RET 4
    0x33, 0xC0, 0xC2, 0x04, 0x00
#endif
};

class MessageBoxThread final
{
public:
    MessageBoxThread(const std::string& text)
        : m_text(text)
        , m_result(IDRETRY)
    {
    }

    int askUser()
    {
        std::thread messageBoxThread = std::thread(&MessageBoxThread::run, this);
        messageBoxThread.join();

        return m_result;
    }

protected:

    void run()
    {
        
#if defined(QT_DEBUG)
        long messageBoxType = MB_RETRYCANCEL;
#else
        long messageBoxType = MB_OK;
#endif

        messageBoxType = messageBoxType | MB_ICONSTOP | MB_TOPMOST;

        m_result = MessageBoxA(0, m_text.c_str(), "SeoSpiderServiceApi Debug Library!", messageBoxType);
    }

private:
    int m_result;
    const std::string m_text;
};

}

namespace SeoSpiderServiceApi
{

SeoSpiderServiceApiImpl* SeoSpiderServiceApiImpl::s_self = nullptr;

SeoSpiderServiceApiImpl::SeoSpiderServiceApiImpl()
    : m_startupInfo{ sizeof(m_startupInfo) }
    , m_initialized(false)
{
    Q_ASSERT(s_self == nullptr && "Double CrashHandler instances detected!");

    s_self = this;
}

void SeoSpiderServiceApiImpl::init() noexcept
{
    if (m_initialized)
    {
        return;
    }

    const boost::uuids::uuid uuid = boost::uuids::random_generator()();
    const std::wstring crashHandlerEventName = boost::lexical_cast<std::wstring>(uuid);

    std::wstring commandLine = L"seospiderservice.exe " + crashHandlerEventName + L" " + std::to_wstring(GetCurrentProcessId());

    m_crashEventHandle = CreateEventW(nullptr, TRUE, FALSE, crashHandlerEventName.c_str());

    m_initialized = CreateProcessW(
        NULL,
        commandLine.data(),
        NULL,
        NULL,
        TRUE,
        CREATE_NO_WINDOW,
        NULL,
        NULL,
        &m_startupInfo,
        &m_processInfo
    );

    if (!m_initialized)
    {
        CloseHandle(m_crashEventHandle);
    }
}

void SeoSpiderServiceApiImpl::free() const noexcept
{
    if (!m_initialized)
    {
        return;
    }

    CloseHandle(m_processInfo.hThread);
    CloseHandle(m_processInfo.hProcess);
    CloseHandle(m_crashEventHandle);
}

void SeoSpiderServiceApiImpl::setProcessSignaledState() const noexcept
{
    std::lock_guard<std::mutex> lock(m_mutex);

    Logger::instance()->flush();

    SetEvent(m_crashEventHandle);

    Sleep(INFINITE);
}

void SeoSpiderServiceApiImpl::setProcessExceptionHandlers() const noexcept
{
    SetUnhandledExceptionFilter(SeoSpiderServiceApiImpl::sehHandler);
    lockExceptionFilter();

    _set_purecall_handler(SeoSpiderServiceApiImpl::pureCallHandler);
    _set_new_handler(SeoSpiderServiceApiImpl::newHandler);
    _set_invalid_parameter_handler(SeoSpiderServiceApiImpl::invalidParameterHandler);
    _set_abort_behavior(_CALL_REPORTFAULT, _CALL_REPORTFAULT);

    std::signal(SIGABRT, SeoSpiderServiceApiImpl::sigAbortHandler);
    std::signal(SIGINT, SeoSpiderServiceApiImpl::sigIntHandler);
    std::signal(SIGTERM, SeoSpiderServiceApiImpl::sigTermHandler);
}

void SeoSpiderServiceApiImpl::setThreadExceptionHandlers() const noexcept
{
    typedef void(*SignalHandlerFunctionType)(int);

    set_terminate(SeoSpiderServiceApiImpl::terminateHandler);
    set_unexpected(SeoSpiderServiceApiImpl::unexpectedHandler);

    std::signal(SIGFPE, reinterpret_cast<SignalHandlerFunctionType>(SeoSpiderServiceApiImpl::sigFpeHandler));
    std::signal(SIGILL, SeoSpiderServiceApiImpl::sigIllHandler);
    std::signal(SIGSEGV, SeoSpiderServiceApiImpl::sigSegvHandler);
}

void SeoSpiderServiceApiImpl::doAssert(const char* file, int line, const char* function, const char* expression) const noexcept
{
#ifndef PRODUCTION
    debugReport(file, line, function, expression);
#else
    setProcessSignaledState();
#endif
}

void SeoSpiderServiceApiImpl::debugReport(const char* file, int line, const char* function, const char* expression) const noexcept
{
    std::stringstream text;

    text << std::string("Debug Assertion") << std::endl << std::endl;
    text << std::string("Thread: ") << (ptrdiff_t)QThread::currentThreadId() << std::endl;
    text << std::string("Function: ") << function << std::endl;
    text << std::string("File: ") << file << std::endl;
    text << std::string("Line: ") << line << std::endl << std::endl;
    text << std::string("Expression: ") << expression << std::endl << std::endl;
    text << std::string("Press Retry to debug the application");

    // Run message box in background thread to stop main thread message pump
    MessageBoxThread msgbox(text.str());

    switch (msgbox.askUser())
    {
        case IDRETRY:
        {
            DebugBreak();
            break;
        }
        case IDCANCEL:
        {
            TerminateProcess(GetCurrentProcess(), EXIT_FAILURE);
            break;
        }
    }
}

void SeoSpiderServiceApiImpl::logMessage(const char* message, SeverityLevel level)
{
    Logger::instance()->logMessage(message, level);
}

LONG WINAPI SeoSpiderServiceApiImpl::sehHandler(PEXCEPTION_POINTERS pExceptionPtrs)
{
    s_self->setProcessSignaledState();
    return EXCEPTION_EXECUTE_HANDLER;
}

void SeoSpiderServiceApiImpl::terminateHandler()
{
    s_self->setProcessSignaledState();
}

void SeoSpiderServiceApiImpl::unexpectedHandler()
{
    s_self->setProcessSignaledState();
}

void SeoSpiderServiceApiImpl::pureCallHandler()
{
    s_self->setProcessSignaledState();
}

void SeoSpiderServiceApiImpl::sigAbortHandler(int)
{
    s_self->setProcessSignaledState();
}

void SeoSpiderServiceApiImpl::sigFpeHandler(int code, int subcode)
{
    s_self->setProcessSignaledState();
}

void SeoSpiderServiceApiImpl::sigIntHandler(int)
{
    s_self->setProcessSignaledState();
}

void SeoSpiderServiceApiImpl::sigIllHandler(int)
{
    s_self->setProcessSignaledState();
}

void SeoSpiderServiceApiImpl::sigSegvHandler(int)
{
    s_self->setProcessSignaledState();
}

void SeoSpiderServiceApiImpl::sigTermHandler(int)
{
    s_self->setProcessSignaledState();
}

int SeoSpiderServiceApiImpl::newHandler(size_t)
{
    s_self->setProcessSignaledState();
    return 0;
}

void SeoSpiderServiceApiImpl::invalidParameterHandler(
    const wchar_t* expression,
    const wchar_t* function,
    const wchar_t* file,
    unsigned int line,
    uintptr_t pReserved)
{
    s_self->setProcessSignaledState();
}

bool SeoSpiderServiceApiImpl::lockExceptionFilter()
{
    //
    // We need to patch SetUnhandledExceptionFilter because
    // CRT library calls this function with 0 as a parameter
    // And it clears the already installed SEH handler
    //

    HMODULE kernel32 = GetModuleHandleW(L"KERNEL32.DLL");

    if (kernel32 == nullptr)
    {
        return false;
    }

    void* entryPoint = static_cast<void*>(GetProcAddress(kernel32, "SetUnhandledExceptionFilter"));

    if (entryPoint == nullptr)
    {
        return false;
    }

    DWORD oldProtect = 0;

    if (VirtualProtect(entryPoint, sizeof(s_interceptFunctionOpcodes), PAGE_EXECUTE_READWRITE, &oldProtect) == FALSE)
    {
        return false;
    }

    std::memcpy(entryPoint, s_interceptFunctionOpcodes, sizeof(s_interceptFunctionOpcodes));
    DWORD temp = 0;

    if (VirtualProtect(entryPoint, sizeof(s_interceptFunctionOpcodes), oldProtect, &temp) == FALSE)
    {
        return false;
    }

    return true;
}

}