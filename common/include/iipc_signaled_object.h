#pragma once

namespace Common
{

class IIpcSignaledObject
{
public:
    enum class WakeupReason
    {
        UnknownErrorReason,
        TimeoutReason,
        ObjectSetSignaledStateReason,
        WaitFailedBySystemReason
    };

    virtual ~IIpcSignaledObject() = default;

    virtual bool create(const std::wstring& name) noexcept = 0;
    
    virtual bool open(const std::wstring& name) noexcept = 0;
    
    virtual bool isNull() const noexcept = 0;

    virtual void setSignaledState() noexcept = 0;

    virtual void resetState() noexcept = 0;

    virtual WakeupReason waitFor(std::uint32_t milliseconds) noexcept = 0;
};

}