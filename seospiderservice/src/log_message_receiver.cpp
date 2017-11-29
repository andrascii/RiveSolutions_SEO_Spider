#include "log_message_receiver.h"

namespace SeoSpiderService
{

LogMessageReceiver::LogMessageReceiver(QObject* parent)
    : QObject(parent)
    , m_socket(new QLocalSocket(this))
    , m_message{}
    , m_connectionTimerId{}
{
    VERIFY(connect(m_socket, &QLocalSocket::disconnected, this, &LogMessageReceiver::onConnectionClosed));
    VERIFY(connect(m_socket, &QLocalSocket::readyRead, this, &LogMessageReceiver::readMessage));

    m_socket->connectToServer("LogServerDataAccumulator");
    m_socket->waitForConnected();

    qRegisterMetaType<Message>("Message");

    if (m_socket->state() != QLocalSocket::ConnectedState)
    {
        m_connectionTimerId = startTimer(1000);

        Q_ASSERT(m_connectionTimerId);
    }
}

void LogMessageReceiver::timerEvent(QTimerEvent*)
{
    m_socket->connectToServer("LogServerDataAccumulator");
    m_socket->waitForConnected();

    if (m_socket->state() == QLocalSocket::ConnectedState)
    {
        killTimer(m_connectionTimerId);
    }
}

void LogMessageReceiver::readMessage()
{
    QDataStream stream(m_socket);
    stream.setVersion(QDataStream::Qt_4_0);

    for (;;)
    {
        if (m_message.messageSize == 0)
        {
            if (m_socket->bytesAvailable() < static_cast<int>(sizeof(std::uint64_t) * 2))
            {
                break;
            }

            stream >> m_message.messageSize;

            std::uint64_t severetyLevel = 0;
            stream >> severetyLevel;

            m_message.severityLevel = static_cast<SeoSpiderServiceApi::SeverityLevel>(severetyLevel);
        }

        if (m_socket->bytesAvailable() < m_message.messageSize || stream.atEnd())
        {
            continue;
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
}

void LogMessageReceiver::onConnectionClosed()
{
    if (m_socket)
    {
        m_socket->deleteLater();
        m_socket = nullptr;
    }
}

}