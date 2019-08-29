#include "rpc_factory.h"
#include "irpc_socket.h"
#include "tcp_socket.h"
#include "tcp_server.h"

namespace Common
{

std::shared_ptr<IRpcSocket> RpcFactory::createRpcSocket()
{
#ifdef Q_OS_WIN
    return std::make_shared<IpcSocket>();
#else
    // on Unix
    return std::make_shared<TcpSocket>();
#endif
}

static std::shared_ptr<IRpcServer> createRpcServer()
{
#ifdef Q_OS_WIN
    return std::make_shared<IpcServer>();
#else
    // on Unix
    return std::make_shared<TcpServer>();
#endif
}

}
