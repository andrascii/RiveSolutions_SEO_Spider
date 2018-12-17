#include "command_thread.h"
#include "command.h"
#include "seo_spider_service_app.h"
#include "sys_info.h"

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
					<< s_separator << QString(logData->text) << "\n";
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
			case Common::Command::Counter:
			{
				Common::CounterData* counterData = command.counterData();

				m_countersJsonBody[counterData->name] =
					m_countersJsonBody[counterData->name].toULongLong() + counterData->value;

				break;
			}
			case Common::Command::ApplicationInitialized:
			{
				Common::ApplicationInitializedData* applicationInitializedData = command.applicationInitializedData();

				if (m_statisticsJsonHeader.isEmpty())
				{
					m_statisticsJsonHeader["userID"] = applicationInitializedData->userID;
					m_statisticsJsonHeader["country"] = applicationInitializedData->country;
					m_statisticsJsonHeader["language"] = applicationInitializedData->language;
					m_statisticsJsonHeader["os"] = applicationInitializedData->os;
					m_statisticsJsonHeader["programBittness"] = applicationInitializedData->programBittness;
					m_statisticsJsonHeader["programVersion"] = applicationInitializedData->programVersion;
					m_statisticsJsonHeader["sessionDateTime"] = QDateTime::currentDateTimeUtc();

					SystemInformation sysinfo;
					getSystemMemoryInfo(sysinfo.systemMemory);

					m_statisticsJsonHeader["physicalMemory"] = formatMemorySize(sysinfo.systemMemory.totalPhysical);

					m_counterContainer["Header"] = m_statisticsJsonHeader;
				}
				else
				{
					//TODO: write error log
				}

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


bool CommandThread::writeStatisticsFile(const QString& fileName)
{
	Q_ASSERT(!isRunning() || QThread::currentThread() == thread());

	QFile statisticsFile(fileName);

	if (statisticsFile.open(QIODevice::WriteOnly | QIODevice::Truncate))
	{
		QTextStream out(&statisticsFile);
		out.setCodec("UTF-8");

		m_counterContainer["Counters"] = m_countersJsonBody;

		if (!m_counterContainer.isEmpty())
		{
			QJsonDocument json = QJsonDocument::fromVariant(m_counterContainer);

			out << json.toJson();

			statisticsFile.flush();
			statisticsFile.close();

			return true;
		}

		statisticsFile.flush();
		statisticsFile.close();
		statisticsFile.remove();
	}

	return false;
}

}