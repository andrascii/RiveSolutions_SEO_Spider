#include "process_watch_dog_thread.h"

namespace ScreenshotMaker
{

ProcessWatchDogThread::ProcessWatchDogThread(const QString& processId, QObject* parent)
	: QThread(parent)
{
	DWORD pid = processId.toInt();

	m_processHandle = OpenProcess(PROCESS_QUERY_INFORMATION | PROCESS_VM_READ | PROCESS_TERMINATE | SYNCHRONIZE, FALSE, pid);
}

void ProcessWatchDogThread::run()
{
	WaitForSingleObject(m_processHandle, INFINITE);

	emit processClosed();
}

}