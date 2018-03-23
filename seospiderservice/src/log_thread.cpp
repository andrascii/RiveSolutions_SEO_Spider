#include "log_thread.h"
#include "pipe_message.h"

namespace
{

const QChar s_separator(';');

}

namespace SeoSpiderService
{

LogThread::LogThread(Common::IpcSocket* socket, const QString& file)
	: m_socket(socket)
{
	qRegisterMetaType<Common::PipeMessage>("Common::PipeMessage");

	m_outputFile.setFileName(file);
	m_outputFile.open(QIODevice::WriteOnly);
}

void LogThread::run()
{
	Common::PipeMessage message;

	forever
	{
		const qint64 size = m_socket->readData(reinterpret_cast<char*>(&message), sizeof(Common::PipeMessage));

		if (m_socket->isClosed())
		{
			m_outputFile.flush();
			break;
		}

		if (size < sizeof(Common::PipeMessage))
		{
			qDebug("error reading channel: %ls", m_socket->errorString().utf16());
			break;
		}

		QTextStream stream(&m_outputFile);

		QString messageType;

		if (message.type == Common::PipeMessage::Assert)
		{
			messageType = "Assertion failure: ";
		}
		else if (message.type == Common::PipeMessage::Assert)
		{
			messageType = "Crash: ";
		}

		stream << QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss.zzz")
			<< s_separator << message.threadId
			<< s_separator << message.severityLevel
			<< s_separator << message.line
			<< s_separator << message.file
			<< s_separator << message.function
			<< s_separator << messageType << message.message << "\n";

		stream.flush();

		emit messageReceived(message);
	}
}

}