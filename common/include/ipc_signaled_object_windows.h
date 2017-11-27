#pragma once

#include "iipc_signaled_object.h"

namespace Common
{

class IpcSignaledObjectWindows : public IIpcSignaledObject
{
public:
    IpcSignaledObjectWindows();

    ~IpcSignaledObjectWindows();

    virtual bool create(const std::wstring& name) noexcept override;

    virtual bool open(const std::wstring& name) noexcept override;

    virtual bool isNull() const noexcept override;

    virtual void setSignaledState() noexcept override;

    virtual void resetState() noexcept override;

    virtual WakeupReason waitFor(std::uint32_t milliseconds) noexcept override;

private:
    HANDLE m_eventHandle;

    bool m_isNull;
};

}