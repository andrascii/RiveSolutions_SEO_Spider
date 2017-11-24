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

    Q_SLOT void logMessage(const QString& message);
    Q_SLOT void flush();

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

private:
    QFile m_outputFile;
    QTextStream m_outputStream;
};

}