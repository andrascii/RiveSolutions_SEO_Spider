#include "tcp_socket.h"

namespace
{

const quint64 c_defaultPort = 12345;

}

namespace Common
{

TcpSocket::TcpSocket()
    : m_tcpSocket(std::make_shared<QTcpSocket>())
{
}

TcpSocket::TcpSocket(QTcpSocket* socket)
    : m_tcpSocket(socket)
{
}

bool TcpSocket::connectToServer(const QString& name)
{
    m_tcpSocket->connectToHost(name, c_defaultPort);

    if (!m_tcpSocket->waitForConnected())
    {
        qDebug() << "Cannot connect to " << name << ":" << c_defaultPort;
        return false;
    }

    return true;
}

void TcpSocket::disconnectFromServer()
{
    m_tcpSocket->disconnectFromHost();
}

QString TcpSocket::serverName() const
{
    return m_tcpSocket->peerName();
}

QString TcpSocket::fullServerName() const
{
    return serverName();
}

qint64 TcpSocket::readData(char* data, qint64 maxSize)
{
    return m_tcpSocket->read(data, maxSize);
}

qint64 TcpSocket::writeData(const char* data, qint64 maxSize)
{
    return m_tcpSocket->write(data, maxSize);
}

qint64 TcpSocket::peekData(char* data, qint64 maxSize)
{
    return m_tcpSocket->peek(data, maxSize);
}

qint64 TcpSocket::transactData(const char* inData, qint64 inSize, char* outData, int outSize)
{
    qint64 bytesProceed = 0;

    do
    {
        bytesProceed += writeData(inData + bytesProceed, inSize - bytesProceed);
    }
    while (bytesProceed >= 0 && bytesProceed < inSize);

    bytesProceed = 0;

    do
    {
        bytesProceed += readData(outData + bytesProceed, outSize - bytesProceed);
    }
    while (bytesProceed >= 0 && bytesProceed < inSize);

    return bytesProceed;
}

QString TcpSocket::errorString() const
{
    return m_tcpSocket->errorString();
}

bool TcpSocket::isClosed()
{
    return !m_tcpSocket->isOpen();
}

}
