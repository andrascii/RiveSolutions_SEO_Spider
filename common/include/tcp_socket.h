#pragma once

#include "irpc_socket.h"

namespace Common
{

class IRpcSocket;

class TcpSocket : public IRpcSocket
{
public:
    TcpSocket();
    TcpSocket(QTcpSocket* socket);
    
    virtual bool connectToServer(const QString& name) override;
    virtual void disconnectFromServer() override;
    virtual QString serverName() const override;
    virtual QString fullServerName() const override;
    virtual qint64 readData(char* data, qint64 maxSize) override;
    virtual qint64 writeData(const char* data, qint64 maxSize) override;
    virtual qint64 peekData(char* data, qint64 maxSize) override;
    virtual qint64 transactData(const char* inData, qint64 inSize, char* outData, int outSize) override;
    virtual QString errorString() const override;
    virtual bool isClosed() override;

private:
    std::shared_ptr<QTcpSocket> m_tcpSocket;
};

}
