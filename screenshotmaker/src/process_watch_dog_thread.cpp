#include "stdafx.h"
#include "process_watch_dog_thread.h"

namespace ScreenshotMaker
{

ProcessWatchDogThread::ProcessWatchDogThread(const QString& processId, QObject* parent)
	: QThread(parent)
	, m_targetPid(processId.toInt())
{
}

void ProcessWatchDogThread::run()
{
#ifdef Q_OS_WIN
	HANDLE processHandle = OpenProcess(
		PROCESS_QUERY_INFORMATION |
		PROCESS_VM_READ |
		PROCESS_TERMINATE |
		SYNCHRONIZE,
		FALSE,
		m_targetPid);

	WaitForSingleObject(processHandle, INFINITE);
#else
	waitpid(m_targetPid, nullptr, 0);
#endif

	emit processClosed();
}

}
