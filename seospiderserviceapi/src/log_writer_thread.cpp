#include "log_writer_thread.h"
#include "log_server_data_accumulator.h"

namespace SeoSpiderServiceApi
{

LogWriterThread::LogWriterThread()
	: m_serverDataAccumulator(nullptr)
{
	qRegisterMetaType<SeverityLevel>("SeverityLevel");

	m_outputFile.setFileName(QStringLiteral("log_data.log"));
	m_outputFile.open(QIODevice::WriteOnly);
	m_outputStream.setDevice(&m_outputFile);

#ifndef PRODUCTION

	m_serverDataAccumulator = new LogServerDataAccumulator(this);

#endif
}

LogWriterThread::~LogWriterThread()
{
	assertIfCallFromAnotherThread();

	m_outputStream.flush();
}

void LogWriterThread::logMessage(const QString& message, SeverityLevel severityLevel)
{
	Q_UNUSED(severityLevel);

	assertIfCallFromAnotherThread();

	m_outputStream << message << "\n";

#ifndef PRODUCTION

	m_serverDataAccumulator->storeLog(message, severityLevel);

#endif
}

void LogWriterThread::flush()
{
	assertIfCallFromAnotherThread();

	m_outputStream.flush();
}

}