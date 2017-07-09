#include "logger.h"

namespace QuickieWebBot
{
Logger::Logger()
{
	QProcess* loggerProc =  new QProcess();
	loggerProc->start("logger", QIODevice::WriteOnly);
	if (!loggerProc->waitForStarted(2000))
	{
		qDebug() << "logger starting error";
		return;
	}


	socket = new QLocalSocket();
	init();
	socket->connectToServer("Logger", QIODevice::WriteOnly);
	if (socket->waitForConnected(10000))
	{
		qDebug() << "Connection initialized!";
		return;
	}
	else
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

void Logger::init()
{
	VERIFY(socket, &QLocalSocket::connected, this, &Logger::connected);
	VERIFY(socket, SIGNAL(QLocalSocket::error(QAbstractSocket::SocketError)), this, &Logger::showError);
	//VERIFY(socket, &QLocalSocket::connected, this, &Logger::connected);
	//VERIFY(socket, &QLocalSocket::connectionClosed, this, &Logger::connectionClosedByServer);
	//VERIFY(socket, &QLocalSocket::error(int), this, &Logger::error(int));
}

void Logger::showError()
{
	qDebug() << "Socket error: " << socket->errorString();
}

void Logger::connected()
{
	m_connected = true;
}

void Logger::sendMessage()
{
}

void Logger::log(MessageType type, QString tag, QString text) const noexcept
{
	//QByteArray message;
	//QString ctype = static_cast<char>(static_cast<int>(type) + 48);
	//message += ctype + "|" + tag + "|" + text + "\n";
	//m_loggerProc->waitForReadyRead(5);
	//m_loggerProc->write(message);

	QByteArray block;
	QDataStream out(&block, QIODevice::WriteOnly);
	out.setVersion(QDataStream::Qt_4_0);
	out << (quint16)0;
	out << tag + "|" + text;
	out.device()->seek(0);
	out << (quint16)(block.size() - sizeof(quint16));
	socket->write(block);

	
}
}