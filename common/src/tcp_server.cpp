#include "tcp_server.h"
#include "tcp_socket.h"

namespace
{

const qint64 c_defaultPort = 12345;

}

namespace Common
{

bool TcpServer::listen(const QString& name)
{
    Q_UNUSED(name);
    QEventLoop loop;
    QObject::connect(&m_tcpServer, &QTcpServer::newConnection, &loop, &QEventLoop::quit);
    m_tcpServer.listen(QHostAddress(name), c_defaultPort);
    loop.exec();
    return true;
}

std::shared_ptr<IRpcSocket> TcpServer::nextPendingConnection()
{
    return std::make_shared<TcpSocket>(m_tcpServer.nextPendingConnection());
}

QString TcpServer::serverName() const
{
    return m_tcpServer.serverAddress().toString();
}

QString TcpServer::fullServerName() const
{
    return serverName();
}

QString TcpServer::errorString() const
{
    return m_tcpServer.errorString();
}

}
