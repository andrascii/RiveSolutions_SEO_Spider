#pragma once

#include "ilogger.h"

namespace SeoSpiderServiceApi
{

class LogServerDataAccumulator;

class LogWriterThread : public QObject
{
    Q_OBJECT

public:
    LogWriterThread();
    ~LogWriterThread();

public slots:
    void flush();

private slots:
    void logMessage(const QString& message, SeverityLevel severityLevel);

private:
    inline void assertIfCallFromAnotherThread() const
    {
        DEBUG_ASSERT(thread() == QThread::currentThread());
    }

private:
    QFile m_outputFile;
    
    QTextStream m_outputStream;

    LogServerDataAccumulator* m_serverDataAccumulator;
};

}