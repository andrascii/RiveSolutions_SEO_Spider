#pragma once

#include "ilogger.h"

namespace SeoSpiderServiceApi
{

class LogWriterThread : public QObject
{
    Q_OBJECT

public:
    LogWriterThread();
    ~LogWriterThread();

private slots:
    void logMessage(const QString& message, SeverityLevel severityLevel);
    void flush();

private:
    inline void abortIfCallFromAnotherThread() const
    {
        if (thread() != QThread::currentThread())
        {
            abort();
        }
    }

    inline void abortIfCallFromThisThread() const
    {
        if (thread() == QThread::currentThread())
        {
            abort();
        }
    }

    void ensureEstablishedConnectionWithServiceAppServer();

    void sendMessageToServiceAppServer(const QString& message, SeverityLevel severityLevel);

private:
    QFile m_outputFile;
    QTextStream m_outputStream;
    QLocalSocket* m_socket;
};

}