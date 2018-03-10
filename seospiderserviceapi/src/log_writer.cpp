#include "log_writer.h"
#include "log_server_data_accumulator.h"

namespace SeoSpiderServiceApi
{

LogWriter::LogWriter()
	: m_serverDataAccumulator(nullptr)
{
	qRegisterMetaType<SeverityLevel>("SeverityLevel");

	QDir::setCurrent(qApp->applicationDirPath());
	m_outputFile.setFileName(QStringLiteral("log_data.log"));
	m_outputFile.open(QIODevice::WriteOnly);
	m_outputStream.setDevice(&m_outputFile);

#ifndef PRODUCTION

	m_serverDataAccumulator = new LogServerDataAccumulator(this);

#endif
}

LogWriter::~LogWriter()
{
	assertIfCallFromAnotherThread();

	m_outputStream.flush();
}

void LogWriter::logMessage(const QString& message, SeverityLevel severityLevel)
{
	Q_UNUSED(severityLevel);

	assertIfCallFromAnotherThread();

	m_outputStream << message << "\n";

#ifndef PRODUCTION

	m_serverDataAccumulator->storeLog(message, severityLevel);

#endif
}

void LogWriter::flush()
{
	assertIfCallFromAnotherThread();

	m_outputStream.flush();
}

}