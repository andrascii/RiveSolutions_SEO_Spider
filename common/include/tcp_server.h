#pragma once

#include "irpc_server.h"

namespace Common
{

class TcpServer : public IRpcServer
{
public:
    virtual bool listen(const QString& name) override;
    virtual std::shared_ptr<IRpcSocket> nextPendingConnection() override;
    virtual QString serverName() const override;
    virtual QString fullServerName() const override;
    virtual QString errorString() const override;

private:
    QTcpServer m_tcpServer;
};

}
