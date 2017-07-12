#include "logger.h"

namespace QuickieWebBot
{
Logger::Logger(QObject* parent) : QObject(parent)
{
	QProcess* loggerProc =  new QProcess();
	loggerProc->start("logger.exe", QIODevice::WriteOnly);
	if (!loggerProc->waitForStarted(2000))
	{
		qDebug() << "logger starting error";
		return;
	}
	qDebug() << loggerProc->errorString();


	socket = new QTcpSocket();
	init();
	socket->connectToHost(QHostAddress::LocalHost, 12345);
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
	VERIFY(socket, SIGNAL(connected), this, SLOT(connected()));
	VERIFY(socket, SIGNAL(QTcpSocket::error(QAbstractSocket::SocketError)), this, SLOT(showError()));
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
	qDebug() << "Connection initialized!";
}

void Logger::sendMessage()
{
}

void Logger::log(MessageType type, QString tag, QString text) const noexcept
{

	QByteArray block;
	QString message;
	QDataStream out(&block, QIODevice::WriteOnly);
	out.setVersion(QDataStream::Qt_4_0);
	out << (quint16)0;	
	//message += static_cast<QChar>(static_cast<int>(type) + 48) + '|';
	message += QString("%1").arg(type) + '|';
	message += QDateTime::currentDateTimeUtc().toString() + '|' ;
	message += tag + '|';
	message += text;
	out << message;
	out.device()->seek(0);
	out << (quint16)(block.size() - sizeof(quint16));
	socket->write(block);
	socket->waitForBytesWritten();
}
}