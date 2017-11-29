#include "log_server_data_accumulator.h"
#include "ilogger.h"

namespace SeoSpiderServiceApi
{

LogServerDataAccumulator::LogServerDataAccumulator(QObject* parent)
    : QObject(parent)
    , m_server(new QLocalServer(this))
    , m_currentConnectionSocket(nullptr)
{
    VERIFY(connect(m_server, &QLocalServer::newConnection, this, &LogServerDataAccumulator::onNewConnection));

    if (!m_server->listen(QStringLiteral("LogServerDataAccumulator")))
    {
        QMessageBox::critical(nullptr, tr("Logger Server"),
            tr("Unable to start the server: %1.").arg(m_server->errorString()), QMessageBox::Ok);
    }
}

void LogServerDataAccumulator::onNewConnection()
{
    if (m_currentConnectionSocket)
    {
        m_currentConnectionSocket->abort();
    }

    m_currentConnectionSocket = m_server->nextPendingConnection();

    VERIFY(connect(m_currentConnectionSocket, &QLocalSocket::disconnected, this, &LogServerDataAccumulator::onConnectionClosed));

    sendAllDataToRemoteProcess();
}

void LogServerDataAccumulator::sendMessageToRemoteProcess(const QString& message, SeverityLevel severityLevel)
{
    if (!m_currentConnectionSocket)
    {
        return;
    }

    QByteArray messageBlock;
    QDataStream sendDataStream(&messageBlock, QIODevice::WriteOnly);
    sendDataStream.setVersion(QDataStream::Qt_4_0);

    sendDataStream << static_cast<std::uint64_t>(0);
    sendDataStream << static_cast<std::uint64_t>(severityLevel);
    sendDataStream.writeBytes(message.toStdString().c_str(), message.size());

    sendDataStream.device()->seek(0);
    sendDataStream << static_cast<std::uint64_t>(message.size());

    const qint64 bytesWritten = m_currentConnectionSocket->write(messageBlock);
    m_currentConnectionSocket->waitForBytesWritten();

//     if (bytesWritten == -1 || bytesWritten < messageBlock.size())
//     {
//         m_outputStream << QString("Cannot send data to remote process or not all data were send. Error code: %1\n").arg(m_currentConnectionSocket->error());
//     }

    m_currentConnectionSocket->flush();
}

void LogServerDataAccumulator::sendAllDataToRemoteProcess()
{
    for (const auto&[severityLevel, logDataRange] : m_logs)
    {
        if (severityLevel == -1)
        {
            continue;
        }

        for (const QString& message : logDataRange)
        {
            sendMessageToRemoteProcess(message, static_cast<SeverityLevel>(severityLevel));
        }
    }
}

void LogServerDataAccumulator::storeLog(const QString& message, SeverityLevel severityLevel)
{
    m_logs[AllLevels].push_back(message);
    m_logs[static_cast<int>(severityLevel)].push_back(message);

    sendMessageToRemoteProcess(message, severityLevel);
}

void LogServerDataAccumulator::onConnectionClosed()
{
    if (m_currentConnectionSocket)
    {
        m_currentConnectionSocket->deleteLater();
        m_currentConnectionSocket = nullptr;
    }
}

}