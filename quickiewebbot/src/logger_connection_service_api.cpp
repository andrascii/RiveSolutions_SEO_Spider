#include "logger_connection_service_api.h"

namespace QuickieWebBot
{

LoggerConnectionServiceApi::LoggerConnectionServiceApi(QObject* parent) 
	: QObject(parent)
{
	QProcess* loggerProc = new QProcess();
	loggerProc->start("logger.exe", QIODevice::WriteOnly);

	if (!loggerProc->waitForStarted(2000))
	{
		qDebug() << "logger starting error";
		qDebug() << loggerProc->errorString();
		return;
	}

	m_socket = new QTcpSocket(this);
	m_socket->connectToHost(QHostAddress::LocalHost, 12345);

	if (!m_socket->waitForConnected(10000))
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
	static LoggerConnectionServiceApi logger;
	return &logger;
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