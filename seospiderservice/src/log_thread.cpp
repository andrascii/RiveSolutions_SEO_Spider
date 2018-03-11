#include "log_thread.h"
#include "pipe_message.h"

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

		stream << message.message << "\n";
		stream.flush();

		emit messageReceived(message);
	}
}

}