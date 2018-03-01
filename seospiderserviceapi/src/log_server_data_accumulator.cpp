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

    writeDataToChannel(message, severityLevel);

    //
    // TODO: handle possible errors
    //

    m_currentConnectionSocket->flush();
}

void LogServerDataAccumulator::sendAllDataToRemoteProcess()
{
    if (!m_currentConnectionSocket)
    {
        return;
    }

    for (const auto&[severityLevel, message] : m_logs)
    {
        writeDataToChannel(message, severityLevel);
    }

    m_currentConnectionSocket->flush();
}

void LogServerDataAccumulator::writeDataToChannel(const QString& message, SeverityLevel severityLevel)
{
    QByteArray messageBlock;
    QDataStream sendDataStream(&messageBlock, QIODevice::WriteOnly);
    sendDataStream.setVersion(QDataStream::Qt_4_0);
	
	const std::string messageString = message.toStdString();
	const char* cString = messageString.c_str();

	const size_t messageSize = strlen(cString);
	//DEBUG_ASSERT(message.size() == static_cast<int>(messageSize));

	sendDataStream << static_cast<qint64>(messageSize);
	sendDataStream << static_cast<qint64>(severityLevel);

	sendDataStream.writeBytes(cString, static_cast<uint>(messageSize));

    const qint64 bytesWritten = m_currentConnectionSocket->write(messageBlock);
    m_currentConnectionSocket->waitForBytesWritten();
}

void LogServerDataAccumulator::storeLog(const QString& message, SeverityLevel severityLevel)
{
    m_logs.emplace_back(std::make_pair(severityLevel, message));

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