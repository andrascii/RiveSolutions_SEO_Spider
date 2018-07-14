#include "seo_spider_service_app.h"
#include "fatal_error_dialog.h"
#include "debug_help_dll_loader.h"
#include "qstandardpaths.h"
#include "zippo.h"
#include "smtp_sender.h"
#include "sys_info.h"

namespace SeoSpiderService
{

SeoSpiderServiceApp::SeoSpiderServiceApp(int& argc, char** argv)
	: QApplication(argc, argv)
	, m_dialog(new FatalErrorDialog)
	, m_dbgHelpDllLoader(new DebugHelpDllLoader)
	, m_loggerDebugWindow(new LoggerDebugWindow)
	, m_zippo(new Zippo)
	, m_cmdThread(std::make_unique<CommandThread>(&m_pipeSocket, logFilePath()))
	, m_pendingReportsCount(0)
	, m_compressionIsActive(false)
{
	setWindowIcon(QIcon(":/images/favicon.ico"));

	m_zippo->start();
	init();

	bool processIdConvertion = false;
	m_processId = commandLineParameter(1).toInt(&processIdConvertion);

	VERIFY(connect(this, &SeoSpiderServiceApp::closeServiceApp, this, &SeoSpiderServiceApp::onServiceClose, Qt::QueuedConnection));
	VERIFY(connect(m_zippo, &Zippo::finished, this, &SeoSpiderServiceApp::onCompressingFinished, Qt::QueuedConnection));

	m_processHandle = OpenProcess(PROCESS_QUERY_INFORMATION | PROCESS_VM_READ | PROCESS_TERMINATE | SYNCHRONIZE, FALSE, m_processId);
	m_timerId = startTimer(100);

	Common::SmtpSender::init();
	sendReports();
}

SeoSpiderServiceApp::~SeoSpiderServiceApp()
{
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
		m_cmdThread->start();
	}

	VERIFY(connect(m_cmdThread.get(), SIGNAL(commandReceived(const Common::Command&)),
		this, SLOT(onCommandReceived(Common::Command)), Qt::QueuedConnection));

#ifdef QT_DEBUG
	VERIFY(connect(m_cmdThread.get(), SIGNAL(commandReceived(const Common::Command&)),
		m_loggerDebugWindow.get(), SLOT(onCommandReceived(Common::Command)), Qt::QueuedConnection));

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
	if (WaitForSingleObject(m_processHandle, 0) == WAIT_TIMEOUT)
	{
		return;
	}

	emit closeServiceApp();
}

bool SeoSpiderServiceApp::makeDump(HANDLE processHandle, const void* threadId, const void* exceptionInfo, qint64 exceptionSize) noexcept
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

	MINIDUMP_EXCEPTION_INFORMATION mdeinfo;
	PMINIDUMP_EXCEPTION_INFORMATION mdeinfoPtr = NULL;

	if (exceptionSize == sizeof(EXCEPTION_POINTERS))
	{
		mdeinfo.ThreadId = quintptr(threadId);
		mdeinfo.ClientPointers = TRUE;
		mdeinfo.ExceptionPointers = PEXCEPTION_POINTERS(exceptionInfo);
		mdeinfoPtr = &mdeinfo;
	}

	const bool result = m_dbgHelpDllLoader->writeDump(processHandle, m_processId, dumpFileHandle, MiniDumpNormal, mdeinfoPtr, NULL, NULL);

	CloseHandle(dumpFileHandle);
	m_cmdThread->quit();
	m_cmdThread->wait();

	const QString sysInfoFileName = QDir::cleanPath(dumpPath + QString("/sys_info.txt"));
	writeSysInfoFile(sysInfoFileName);

	QFile::copy(logFilePath(), QDir::cleanPath(dumpPath + QString("/log_data.log")));

	m_compressionIsActive = true;
	m_zippo->zcompress(dumpDir, dumpPath + QString(".zip"), QStringList(), QString());
	m_defferedDeleteDir = dumpDir;

	return result;
}

void SeoSpiderServiceApp::writeSysInfoFile(const QString& fileName) const
{
	QFile sysinfoFile(fileName);

	if (sysinfoFile.exists())
	{
		return;
	}

	if (sysinfoFile.open(QIODevice::WriteOnly | QIODevice::Truncate))
	{
		QTextStream out(&sysinfoFile);
		out.setCodec("UTF-8");

		printSystemInformation(out, m_processId);

		sysinfoFile.flush();
		sysinfoFile.close();
	}
}

void SeoSpiderServiceApp::sendReports()
{
	const QString path = SeoSpiderServiceApp::dumpsPath();
	QDir dir(path);

	QFileInfoList files = dir.entryInfoList();

	Common::SmtpSettings settings;
	settings.setEmailUsername("rivesolutionreports");
	settings.setEmailPassword("Riv3S#lutions");
	settings.setEmailSender("rivesolutionreports@yandex.ru");
	settings.setEmailSmtpPort(465);
	settings.setEmailSmtpHost("smtp.yandex.ru");
	settings.setEmailRecipients("bugreports@rivesolutions.com;kirchet@yandex.ru;pugachev.andrey.work@gmail.com;blackeagleeger8@mail.ru");
	settings.setEmailUseSsl(true);
	settings.setEmailUseAuthentication(true);

	foreach(const QFileInfo& file, files)
	{
		if (!file.fileName().endsWith(".zip"))
		{
			continue;
		}

		++m_pendingReportsCount;

		QString info = m_dialog->detailedInformation();
		QTextCodec* codec = QTextCodec::codecForName("latin1");
		info = codec->makeDecoder()->toUnicode(info.toUtf8());

		Common::SmtpMessage message(settings, "Report", info, { file.absoluteFilePath() });
		Common::SmtpSender::send(message, file.absoluteFilePath(), this, SLOT(onSendingFinished(const QString&, int, const QByteArray&)));
	}

	if (m_dialog->sendReportsNow() && m_pendingReportsCount == 0)
	{
		emit closeServiceApp();
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

QString SeoSpiderServiceApp::logFilePath()
{
	QString path = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation);
	path = QDir::cleanPath(path + QString("/RiveSolutions/SeoSpider"));

	QDir dir(path);

	if (!dir.exists())
	{
		dir.mkpath(path);
	}

	return QDir::cleanPath(path + QString("/log_data.log"));
}

void SeoSpiderServiceApp::onSendingFinished(const QString& mailId, int result, const QByteArray& log)
{
	Q_UNUSED(log);

	if (result == Common::SmtpSender::resultSuccess)
	{
		QFile(mailId).remove();
	}

	--m_pendingReportsCount;

	if (m_dialog->sendReportsNow() && m_pendingReportsCount == 0)
	{
		emit closeServiceApp();
	}
}

void SeoSpiderServiceApp::onCompressingFinished()
{
	m_compressionIsActive = false;
	if (m_dialog->sendReportsNow())
	{
		sendReports();
	}
}

void SeoSpiderServiceApp::onCommandReceived(Common::Command command)
{
	if (!command.dumpData() && !command.assertData())
	{
		return;
	}

	const void* threadId = command.dumpData() ? command.dumpData()->threadId : command.assertData()->threadId;
	const void* exceptionInfo = command.dumpData() ? command.dumpData()->exceptionInfo : command.assertData()->exceptionInfo;
	qint64 exceptionSize = command.dumpData() ? command.dumpData()->exceptionSize : command.assertData()->exceptionSize;

	const bool dumpCreationResult = makeDump(m_processHandle, threadId, exceptionInfo, exceptionSize);

	m_dialog->exec();

	if (m_dialog->sendReportsNow())
	{
		if (!m_compressionIsActive)
		{
			sendReports();
		}

		return;
	}

	Common::Result result;
	result.errorcode = !dumpCreationResult ? GetLastError() : 0;
	m_pipeSocket.writeData(reinterpret_cast<const char*>(&result), sizeof(result));

	if (!m_dialog->isVisible() && !m_dialog->sendReportsNow())
	{
		emit closeServiceApp();
	}
}

void SeoSpiderServiceApp::onServiceClose()
{
	if (m_loggerDebugWindow)
	{
		m_loggerDebugWindow->close();

		processEvents();
	}

	quit();
}

}
