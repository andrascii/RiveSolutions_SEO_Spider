#include "log_message_receiver.h"

namespace SeoSpiderService
{

LogMessageReceiver::LogMessageReceiver(QObject* parent)
    : QObject(parent)
    , m_server(new QLocalServer(this))
    , m_currentConnectionSocket(nullptr)
    , m_message{}
{
    VERIFY(connect(m_server, &QLocalServer::newConnection, this, &LogMessageReceiver::onNewConnection));

    if (!m_server->listen(QStringLiteral("LogMessageReceiver")))
    {
        QMessageBox::critical(nullptr, tr("Logger Server"), 
            tr("Unable to start the server: %1.").arg(m_server->errorString()), QMessageBox::Ok);
    }

    qRegisterMetaType<Message>("Message");
}

void LogMessageReceiver::onNewConnection()
{
    if (m_currentConnectionSocket)
    {
        m_currentConnectionSocket->abort();
    }

    m_currentConnectionSocket = m_server->nextPendingConnection();

    connect(m_currentConnectionSocket, &QLocalSocket::disconnected, this, &LogMessageReceiver::onConnectionClosed);
    connect(m_currentConnectionSocket, &QLocalSocket::readyRead, this, &LogMessageReceiver::readMessage);
}

void LogMessageReceiver::readMessage()
{
    QDataStream stream(m_currentConnectionSocket);
    stream.setVersion(QDataStream::Qt_4_0);
    stream.device()->seek(0);

    if (m_message.messageSize == 0)
    {
        if (m_currentConnectionSocket->bytesAvailable() < static_cast<int>(sizeof(std::uint64_t) * 2))
        {
            return;
        }

        stream >> m_message.messageSize;

        std::uint64_t severetyLevel = 0;
        stream >> severetyLevel;

        m_message.severityLevel = static_cast<SeoSpiderServiceApi::SeverityLevel>(severetyLevel);
    }

    const int allMessageSize = static_cast<int>(m_message.messageSize + sizeof(std::uint64_t) * 2);

    if (m_currentConnectionSocket->bytesAvailable() < allMessageSize || stream.atEnd())
    {
        return;
    }

    char* buffer = new char[m_message.messageSize + 1];

    uint length = static_cast<uint>(m_message.messageSize);

    stream.readBytes(buffer, length);
    buffer[m_message.messageSize] = 0;

    m_message.message = buffer;

    delete[] buffer;

    emit messageReceived(m_message);

    m_message = Message{};
}

void LogMessageReceiver::onConnectionClosed()
{
    if (m_currentConnectionSocket)
    {
        m_currentConnectionSocket->deleteLater();
        m_currentConnectionSocket = nullptr;
    }
}

}