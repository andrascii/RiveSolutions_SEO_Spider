#include "named_thread.h"

#if defined (Q_OS_WIN)
#define USE_WINDOWS_SET_THREAD_NAME_HACK
#endif

#if defined (USE_WINDOWS_SET_THREAD_NAME_HACK)

namespace
{

#pragma pack(push,8)
	typedef struct tagTHREADNAME_INFO
	{
		DWORD dwType; // Must be 0x1000.
		LPCSTR szName; // Pointer to name (in user addr space).
		DWORD dwThreadID; // Thread ID (-1=caller thread).
		DWORD dwFlags; // Reserved for future use, must be zero.
	} THREADNAME_INFO;
#pragma pack(pop)

	void setThreadName(DWORD threadID, const QByteArray& threadName)
	{
		THREADNAME_INFO info;
		info.dwType = 0x1000;
		info.szName = threadName;
		info.dwThreadID = threadID;
		info.dwFlags = 0;

		__try
		{
			const DWORD MS_VC_EXCEPTION = 0x406D1388;
			RaiseException(MS_VC_EXCEPTION, 0, sizeof(info) / sizeof(ULONG_PTR), (ULONG_PTR*)&info);
		}
		__except (EXCEPTION_EXECUTE_HANDLER)
		{
		}
	}

	void setCurrentThreadName(const QByteArray& threadName)
	{
		setThreadName(-1, threadName);
	}

}

#endif

namespace Common
{

	NamedThread::NamedThread(const QByteArray& name, QObject* parent)
		: QThread(parent)
	{
		setObjectName(name);
	}

	void NamedThread::run()
	{
#ifdef USE_WINDOWS_SET_THREAD_NAME_HACK
		setCurrentThreadName(objectName().toLatin1());
#endif
		QThread::run();
	}

}