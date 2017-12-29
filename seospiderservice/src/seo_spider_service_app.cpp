#include "seo_spider_service_app.h"
#include "fatal_error_dialog.h"
#include "debug_help_dll_loader.h"
#include "log_message_receiver.h"
#include "iipc_signaled_object.h"
#include "ipc_signaled_object_creator.h"

namespace SeoSpiderService
{

SeoSpiderServiceApp::SeoSpiderServiceApp(int& argc, char** argv)
	: QApplication(argc, argv)
	, m_dialog(new FatalErrorDialog)
	, m_dbgHelpDllLoader(new DebugHelpDllLoader)
	, m_loggerDebugWindow(new LoggerDebugWindow)
{
	init();

	bool processIdConvertion = false;

	m_eventName = commandLineParameter(1).toLatin1();
	m_processId = commandLineParameter(2).toInt(&processIdConvertion);

	Q_ASSERT(processIdConvertion && "Process ID must be passed!");

	VERIFY(connect(this, &SeoSpiderServiceApp::closeServiceApp, this, &SeoSpiderServiceApp::quit, Qt::QueuedConnection));

	m_signaledEvent = OpenEvent(SYNCHRONIZE, FALSE, m_eventName.constData());
	m_processHandle = OpenProcess(PROCESS_QUERY_INFORMATION | PROCESS_VM_READ | PROCESS_TERMINATE | SYNCHRONIZE, FALSE, m_processId);

	m_timerId = startTimer(100);
}

SeoSpiderServiceApp::~SeoSpiderServiceApp()
{
	CloseHandle(m_signaledEvent);
	CloseHandle(m_processHandle);
}

void SeoSpiderServiceApp::init()
{
#ifdef QT_DEBUG
	LogMessageReceiver* logMessageReceiver = new LogMessageReceiver;

	QThread* thread = new QThread;
	logMessageReceiver->moveToThread(thread);
	thread->start();

	VERIFY(connect(logMessageReceiver, SIGNAL(messageReceived(Message)),
		m_loggerDebugWindow.get(), SLOT(onMessageReceived(Message))));

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
	const QString dumpFileName = "process_dump.dmp";

	HANDLE dumpFileHandle = CreateFileW(dumpFileName.toStdWString().c_str(),
		GENERIC_WRITE, FILE_SHARE_DELETE | FILE_SHARE_READ | FILE_SHARE_WRITE,
		NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);

	MINIDUMP_TYPE miniDumpType = MINIDUMP_TYPE::MiniDumpWithFullMemory;

	m_dbgHelpDllLoader->writeDump(processHandle, m_processId, dumpFileHandle, miniDumpType, NULL, NULL, NULL);

	CloseHandle(dumpFileHandle);
}

}