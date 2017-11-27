#include "ipc_signaled_object_creator.h"
#include "iipc_signaled_object.h"

#if defined(Q_OS_WIN)
#include "ipc_signaled_object_windows.h"
#else
#include "ipc_signaled_object_unix_like.h"
#endif

namespace Common
{

SignaledObjectCreator creator()
{
#if defined(Q_OS_WIN)

    return SignaledObjectCreator([] { return std::unique_ptr<IIpcSignaledObject>(new IpcSignaledObjectWindows); });

#elif defined(Q_OS_UNIX) || defined(Q_OS_LINUX) || defined(Q_OS_MAC)

    return SignaledObjectCreator([] { return std::unique_ptr<IIpcSignaledObject>(new IpcSignaledObjectUnixLike); });

#endif

    return SignaledObjectCreator([] { return std::unique_ptr<IIpcSignaledObject>(nullptr); });
}

}