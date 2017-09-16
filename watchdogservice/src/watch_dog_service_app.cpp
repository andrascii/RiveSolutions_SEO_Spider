#include "watch_dog_service_app.h"
#include "fatal_error_dialog.h"
#include "debug_help_dll_loader.h"

namespace WatchDogService
{

WatchDogServiceApp::WatchDogServiceApp(int& argc, char** argv)
	: QApplication(argc, argv)
	, m_dialog(new FatalErrorDialog)
	, m_dbgHelpDllLoader(new DebugHelpDllLoader)
{
	bool processIdConvertion = false;

	m_eventName = commandLineParameter(1).toLatin1();
	m_processId = commandLineParameter(2).toInt(&processIdConvertion);

	ASSERT(!m_eventName.isEmpty() && "Event name must be passed!");
	ASSERT(processIdConvertion && "Process ID must be passed!");

	VERIFY(connect(this, &WatchDogServiceApp::closeServiceApp, this, &WatchDogServiceApp::quit, Qt::QueuedConnection));

	QMetaObject::invokeMethod(this, "waitForSignaledEvent", Qt::QueuedConnection);
}

QString WatchDogServiceApp::commandLineParameter(int num) const noexcept
{
	const QStringList commandLine = qApp->arguments();
	
	if (num < commandLine.size())
	{
		return commandLine[num];
	}

	return QString();
}

void WatchDogServiceApp::waitForSignaledEvent() const noexcept
{
	HANDLE signaledEvent = OpenEvent(SYNCHRONIZE, FALSE, m_eventName.constData());
	HANDLE processHandle = OpenProcess(PROCESS_QUERY_INFORMATION | PROCESS_VM_READ | PROCESS_TERMINATE | SYNCHRONIZE, FALSE, m_processId);

	HANDLE waitableHandles[] = { processHandle, signaledEvent };

	DWORD awakenedObject = WaitForMultipleObjects(sizeof(waitableHandles) / sizeof(HANDLE), waitableHandles, FALSE, INFINITE);

	switch (awakenedObject)
	{
		case WAIT_OBJECT_0 + 0:
		{
			break;
		}
		case WAIT_OBJECT_0 + 1:
		{
			makeCrashDump(processHandle);

			TerminateProcess(processHandle, 0xDEAD);

			m_dialog->exec();
		}
	}

	CloseHandle(signaledEvent);
	CloseHandle(processHandle);

	emit closeServiceApp();
}

void WatchDogServiceApp::makeCrashDump(HANDLE processHandle) const noexcept
{
	const QString dumpFileName = "qwbcrash.dmp";

	HANDLE dumpFileHandle = CreateFileW(dumpFileName.toStdWString().c_str(),
		GENERIC_WRITE, FILE_SHARE_DELETE | FILE_SHARE_READ | FILE_SHARE_WRITE,
		NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);

	MINIDUMP_TYPE miniDumpType = MINIDUMP_TYPE::MiniDumpNormal;

	m_dbgHelpDllLoader->writeDump(processHandle, m_processId, dumpFileHandle, miniDumpType, NULL, NULL, NULL);

	CloseHandle(dumpFileHandle);
}

}