#include "logger.h"

namespace QuickieWebBot
{

Logger::Logger(QObject* parent) 
	: QObject(parent)
{
	QProcess* loggerProc = new QProcess(this);
	loggerProc->start("logger.exe", QIODevice::WriteOnly);

	if (!loggerProc->waitForStarted(2000))
	{
		qDebug() << "logger starting error";
		qDebug() << loggerProc->errorString();
		return;
	}

	m_socket = new QTcpSocket();
	m_socket->connectToHost(QHostAddress::LocalHost, 12345);

	if (!m_socket->waitForConnected(10000))
	{
		qDebug() << "Connection timeout!";
		return;
	}
}

Logger* Logger::instance()
{
	static Logger logger;
	return &logger;
}

void Logger::log(MessageType type, QString tag, QString text, QString func) const noexcept
{
	std::lock_guard<std::mutex> locker(m_mutex);

	QByteArray block;
	QString message;

	QDataStream out(&block, QIODevice::WriteOnly);
	out.setVersion(QDataStream::Qt_4_0);

	out << static_cast<quint16>(0);

	message += QString("%1 | %2 | %3 | %4 | %5")
		.arg(type)
		.arg(QDateTime::currentDateTime().toString())
		.arg(func)
		.arg(tag)
		.arg(text);
	
	out << message;
 	out.device()->seek(0);

	out << static_cast<quint16>(block.size() - sizeof(quint16)) << message;

	m_socket->write(block);
	m_socket->waitForBytesWritten();
}

}