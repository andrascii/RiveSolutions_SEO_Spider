#include "log_writer_thread.h"
#include "named_thread.h"

namespace SeoSpiderServiceApi
{

LogWriterThread::LogWriterThread()
{
    m_outputFile.setFileName(QStringLiteral("log_data.log"));
    m_outputFile.open(QIODevice::WriteOnly);
    m_outputStream.setDevice(&m_outputFile);

    Common::NamedThread* thread = new Common::NamedThread("LogWriterThread");

    connect(qApp, &QApplication::aboutToQuit, this, &LogWriterThread::flush, Qt::QueuedConnection);
    connect(thread, &QThread::finished, this, &LogWriterThread::flush, Qt::QueuedConnection);

    moveToThread(thread);
    thread->start();
}

LogWriterThread::~LogWriterThread()
{
    abortIfCallFromThisThread();

    thread()->quit();
    thread()->wait();

    delete thread();
}

void LogWriterThread::logMessage(const QString& message)
{
    abortIfCallFromAnotherThread();

    m_outputStream << message << "\n";
}

void LogWriterThread::flush()
{
    abortIfCallFromAnotherThread();

    m_outputStream.flush();
}

}