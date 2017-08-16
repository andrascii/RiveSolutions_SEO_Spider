#include "logger_connection_service_api.h"

namespace QuickieWebBot
{

LoggerConnectionServiceApi::LoggerConnectionServiceApi(QObject* parent) 
	: QObject(parent)
{
	QProcess* loggerProc = new QProcess(this);
	loggerProc->start("logger.exe", QIODevice::WriteOnly);

	if (!loggerProc->waitForStarted())
	{
		qDebug() << "logger starting error";
		qDebug() << loggerProc->errorString();
		return;
	}

	m_socket = new QTcpSocket(this);
	m_socket->connectToHost(QHostAddress::LocalHost, 12345);

	if (!m_socket->waitForConnected())
	{
		qDebug() << "Connection timeout!";
		return;
	}
}

LoggerConnectionServiceApi::~LoggerConnectionServiceApi()
{
	m_socket->disconnectFromHost();	
}

LoggerConnectionServiceApi* LoggerConnectionServiceApi::instance()
{
	static std::unique_ptr<LoggerConnectionServiceApi> logger;

	if (!logger)
	{
		logger.reset(new LoggerConnectionServiceApi());
	}

	return logger.get();
}

LoggerConnectionServiceApi::LoggerDataStream LoggerConnectionServiceApi::log(MessageType type, QString func, QString file, int line)
{
	return LoggerDataStream(type, func, file, line);
}

LoggerConnectionServiceApi::LoggerDataStream::~LoggerDataStream()
{
	LoggerConnectionServiceApi::instance()->log(this->m_messageType, this->m_fucntionName, this->m_fileName, this->m_lineNumber, this->m_message.trimmed());
}

}