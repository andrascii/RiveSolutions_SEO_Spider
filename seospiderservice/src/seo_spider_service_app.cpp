#include "seo_spider_service_app.h"
#include "fatal_error_dialog.h"
#include "debug_help_dll_loader.h"
#include "log_message_receiver.h"
#include "iipc_signaled_object.h"
#include "ipc_signaled_object_creator.h"
#include "qstandardpaths.h"
#include "zippo.h"
#include "smtp_sender.h"

namespace SeoSpiderService
{

SeoSpiderServiceApp::SeoSpiderServiceApp(int& argc, char** argv)
	: QApplication(argc, argv)
	, m_dialog(new FatalErrorDialog)
	, m_dbgHelpDllLoader(new DebugHelpDllLoader)
	, m_loggerDebugWindow(new LoggerDebugWindow)
	, m_zippo(new Zippo)
	, m_logThread(std::make_unique<LogThread>(&m_pipeSocket))
{
	m_zippo->start();
	init();

	bool processIdConvertion = false;

	m_eventName = commandLineParameter(1).toLatin1();
	m_processId = commandLineParameter(2).toInt(&processIdConvertion);

	Q_ASSERT(processIdConvertion && "Process ID must be passed!");

	VERIFY(connect(this, &SeoSpiderServiceApp::closeServiceApp, this, &SeoSpiderServiceApp::quit, Qt::QueuedConnection));

	//m_crashEventSignaledObject->open(m_eventName.constData());
	m_signaledEvent = OpenEvent(SYNCHRONIZE, FALSE, m_eventName.constData());
	m_processHandle = OpenProcess(PROCESS_QUERY_INFORMATION | PROCESS_VM_READ | PROCESS_TERMINATE | SYNCHRONIZE, FALSE, m_processId);

	m_timerId = startTimer(100);

	Common::SmtpSender::init();
	sendReports();
}

SeoSpiderServiceApp::~SeoSpiderServiceApp()
{
	CloseHandle(m_signaledEvent);
	CloseHandle(m_processHandle);

	m_zippo->quit();
	m_zippo->wait();
	
	if (m_defferedDeleteDir != std::nullopt)
	{
		m_defferedDeleteDir->removeRecursively();
	}
}

void SeoSpiderServiceApp::init()
{
	using namespace std::chrono_literals;

	constexpr int c_maxConnectionAttemptCount = 5;

	bool connectionResult = false;

	for (int i = 0; i < c_maxConnectionAttemptCount; ++i)
	{
		connectionResult = m_pipeSocket.connectToServer(QString("seospiderserviceapi_log_channel"), QIODevice::ReadWrite);

		if (connectionResult)
		{
			break;
		}

		std::this_thread::sleep_for(1s);
	}

	if (connectionResult)
	{
		m_logThread->start();
	}

#ifdef QT_DEBUG
	VERIFY(connect(m_logThread.get(), SIGNAL(messageReceived(const Common::PipeMessage&)),
		m_loggerDebugWindow.get(), SLOT(onMessageReceived(const Common::PipeMessage&)), Qt::QueuedConnection));

	m_loggerDebugWindow->show();
#endif
}

QString SeoSpiderServiceApp::commandLineParameter(int num) const noexcept
{
	const QStringList commandLine = qApp->arguments();

	if (num < commandLine.size())
	{
		return commandLine[num];
	}

	return QString();
}

void SeoSpiderServiceApp::timerEvent(QTimerEvent*)
{
	HANDLE waitableHandles[] = { m_processHandle, m_signaledEvent };
	DWORD awakenedObject = WaitForMultipleObjects(sizeof(waitableHandles) / sizeof(HANDLE), waitableHandles, FALSE, 1);

	switch (awakenedObject)
	{
		case WAIT_OBJECT_0 + 0:
		{
			break;
		}
		case WAIT_OBJECT_0 + 1:
		{
			makeDump(m_processHandle);

			TerminateProcess(m_processHandle, 0xDEAD);

			m_dialog->exec();

			break;
		}
		case WAIT_TIMEOUT:
		{
			return;
		}
	}

	emit closeServiceApp();
}

void SeoSpiderServiceApp::makeDump(HANDLE processHandle) const noexcept
{
	const QString path = SeoSpiderServiceApp::dumpsPath();

	const QString dumpId = QDateTime::currentDateTime().toString(QString("ddMMyyyyHHmmss"));
	const QString dumpPath = QDir::cleanPath(path + QString("/") + dumpId);

	QDir dumpDir(dumpPath);
	if (!dumpDir.exists())
	{
		dumpDir.mkpath(dumpPath);
	}

	const QString dumpFileName = QDir::cleanPath(dumpPath + QString("/process_dump.dmp"));

	HANDLE dumpFileHandle = CreateFileW(dumpFileName.toStdWString().c_str(),
		GENERIC_WRITE, FILE_SHARE_DELETE | FILE_SHARE_READ | FILE_SHARE_WRITE,
		NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);

	MINIDUMP_TYPE miniDumpType = MINIDUMP_TYPE::MiniDumpNormal;

	m_dbgHelpDllLoader->writeDump(processHandle, m_processId, dumpFileHandle, miniDumpType, NULL, NULL, NULL);

	CloseHandle(dumpFileHandle);
	m_zippo->zcompress(dumpDir, dumpPath + QString(".zip"), QStringList(), QString());
	m_defferedDeleteDir = dumpDir;
}

void SeoSpiderServiceApp::sendReports()
{
	QThread::sleep(15);
	const QString path = SeoSpiderServiceApp::dumpsPath();
	QDir dir(path);

	QFileInfoList files = dir.entryInfoList();

	Common::SmtpSettings settings;
	settings.setEmailUsername("rivesolutionreports@yandex.ru");
	settings.setEmailPassword("riv3soLutions!");
	settings.setEmailSender("rivesolutionreports@yandex.ru");
	settings.setEmailSmtpPort(465);
	settings.setEmailSmtpHost("smtp.yandex.ru");
	settings.setEmailRecipients("kirchet@yandex.ru");
	settings.setEmailUseSsl(false);
	settings.setEmailUseAuthentication(true);



	foreach(const QFileInfo& file, files)
	{
		if (!file.fileName().endsWith(".zip"))
		{
			continue;
		}

		
		Common::SmtpMessage message(settings, "Report", "Message Text", { file.absoluteFilePath() });
		Common::SmtpSender::send(message);
		//QFile(file.absoluteFilePath()).remove();
	}

}

QString SeoSpiderServiceApp::dumpsPath()
{
	QString path = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation);
	path = QDir::cleanPath(path + QString("/RiveSolutions/SeoSpider/dumps"));

	QDir dir(path);

	if (!dir.exists())
	{
		dir.mkpath(path);
	}

	return path;
}

}
