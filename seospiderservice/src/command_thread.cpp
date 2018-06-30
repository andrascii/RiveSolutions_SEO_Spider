#include "command_thread.h"
#include "command.h"

namespace
{

const QChar s_separator(';');

}

namespace SeoSpiderService
{

CommandThread::CommandThread(Common::IpcSocket* socket, const QString& file)
	: m_socket(socket)
{
	qRegisterMetaType<Common::Command>("Common::Command");

	m_outputFile.setFileName(file);
	m_outputFile.open(QIODevice::WriteOnly);
}

void CommandThread::run()
{
	Common::Command command;

	forever
	{
		if (m_socket->isClosed())
		{
			break;
		}

		const qint64 size = m_socket->readData(reinterpret_cast<char*>(&command), sizeof(Common::Command));

		if (size < sizeof(Common::Command))
		{
			qDebug("error reading channel: %ls", m_socket->errorString().utf16());
			break;
		}

		QTextStream stream(&m_outputFile);
		QString messageType;

		switch (command.type)
		{
			case Common::Command::Assert:
			{
				Common::AssertData* assertData = command.assertData();

				stream << QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss.zzz")
					<< s_separator << assertData->file
					<< s_separator << assertData->line
					<< s_separator << assertData->function
					<< s_separator << quintptr(assertData->thisptr)
					<< s_separator << assertData->processId
					<< s_separator << quintptr(assertData->threadId)
					<< s_separator << "ASSERTION FAILURE: " << assertData->condition << "\n";
				break;
			}
			case Common::Command::Dump:
			{
				Common::DumpData* dumpData = command.dumpData();

				stream << QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss.zzz")
					<< s_separator << dumpData->processId
					<< s_separator << quintptr(dumpData->threadId)
					<< s_separator << (dumpData->dumpParams == Common::DumpData::NativeCrash ? "NATIVE CRASH" : "USER DUMP") << "\n";
				break;
			}
			case Common::Command::Log:
			{
				Common::LogData* logData = command.logData();

				stream << QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss.zzz")
					<< s_separator << logData->categoryId
					<< s_separator << logData->level
					<< s_separator << logData->file
					<< s_separator << logData->line
					<< s_separator << logData->function
					<< s_separator << quintptr(logData->thisptr)
					<< s_separator << quintptr(logData->threadId)
					<< s_separator << logData->text << "\n";
				break;
			}
			case Common::Command::LogCategory:
			{
				break;
			}
			case Common::Command::Restart:
			{
				Common::RestartData* restartData = command.restartData();

				stream << QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss.zzz")
					<< s_separator << "RESTART REQUEST: " << restartData->message << "\n";
				break;
			}
		}

		stream.flush();

		emit commandReceived(command);

		if (command.type == Common::Command::Assert ||
			command.type == Common::Command::Dump ||
			command.type == Common::Command::Restart)
		{
			break;
		}
	}

	m_outputFile.flush();
}

}