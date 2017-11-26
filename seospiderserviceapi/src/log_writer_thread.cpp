#include "log_writer_thread.h"
#include "named_thread.h"

namespace SeoSpiderServiceApi
{

LogWriterThread::LogWriterThread()
    : m_socket(new QLocalSocket(this))
{
    qRegisterMetaType<SeverityLevel>("SeverityLevel");

    m_outputFile.setFileName(QStringLiteral("log_data.log"));
    m_outputFile.open(QIODevice::WriteOnly);
    m_outputStream.setDevice(&m_outputFile);

    Common::NamedThread* thread = new Common::NamedThread("LogWriterThread");

    connect(qApp, &QApplication::aboutToQuit, this, &LogWriterThread::flush, Qt::QueuedConnection);
    connect(thread, &QThread::finished, this, &LogWriterThread::flush, Qt::QueuedConnection);

    moveToThread(thread);
    thread->start();

#ifndef PRODUCTION

    ensureEstablishedConnectionWithServiceAppServer();

#endif
}

LogWriterThread::~LogWriterThread()
{
    abortIfCallFromThisThread();

    m_outputStream.flush();

    thread()->quit();
    thread()->wait();

    delete thread();
}

void LogWriterThread::logMessage(const QString& message, SeverityLevel severityLevel)
{
    abortIfCallFromAnotherThread();

    m_outputStream << message << "\n";

#ifndef PRODUCTION

    sendMessageToServiceAppServer(message, severityLevel);

#endif
}

void LogWriterThread::flush()
{
    abortIfCallFromAnotherThread();

    m_outputStream.flush();
}

void LogWriterThread::ensureEstablishedConnectionWithServiceAppServer()
{
#ifndef PRODUCTION

    if (m_socket->state() == QAbstractSocket::ConnectedState)
    {
        return;
    }

    if (m_socket->state() == QAbstractSocket::ConnectingState)
    {
        m_socket->waitForConnected();
        return;
    }

    m_socket->connectToServer("LogMessageReceiver");

    QString connectionStateLogMessage;

    if (m_socket->waitForConnected())
    {
        connectionStateLogMessage = "Connected to server!";
    }
    else
    {
        connectionStateLogMessage = QString("Cannot connect to server or timeout. Error code: %1").arg(m_socket->error());
    }

    QMetaObject::invokeMethod(this, "logMessage", Qt::QueuedConnection,
        Q_ARG(const QString&, connectionStateLogMessage), Q_ARG(SeverityLevel, SeverityLevel::DebugLevel));

#endif
}

void LogWriterThread::sendMessageToServiceAppServer(const QString& message, SeverityLevel severityLevel)
{
    ensureEstablishedConnectionWithServiceAppServer();

#ifndef PRODUCTION

    QByteArray messageBlock;
    QDataStream sendDataStream(&messageBlock, QIODevice::WriteOnly);
    sendDataStream.setVersion(QDataStream::Qt_4_0);

    sendDataStream << static_cast<std::uint64_t>(0);
    sendDataStream << static_cast<std::uint64_t>(severityLevel);
    sendDataStream.writeBytes(message.toStdString().c_str(), message.size());

    sendDataStream.device()->seek(0);
    sendDataStream << static_cast<std::uint64_t>(message.size());

    const qint64 bytesWritten = m_socket->write(messageBlock);
    m_socket->waitForBytesWritten();

    if (bytesWritten == -1 || bytesWritten < messageBlock.size())
    {
        m_outputStream << QString("Cannot send data to server or not all data were send. Error code: %1\n").arg(m_socket->error());
    }

    m_socket->flush();

#endif
}

}