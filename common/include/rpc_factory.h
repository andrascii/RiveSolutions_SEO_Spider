#pragma once

namespace Common
{

class IRpcSocket;
class IRpcServer;

class RpcFactory
{
public:
    static std::shared_ptr<IRpcSocket> createRpcSocket();
    static std::shared_ptr<IRpcServer> createRpcServer();
};

}
