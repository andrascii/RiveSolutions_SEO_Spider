#include "seo_spider_service_app.h"
#include "fatal_error_dialog.h"
#include "debug_help_dll_loader.h"

namespace SeoSpiderService
{

SeoSpiderServiceApp::SeoSpiderServiceApp(int& argc, char** argv)
    : QApplication(argc, argv)
    , m_dialog(new FatalErrorDialog)
    , m_dbgHelpDllLoader(new DebugHelpDllLoader)
{
    setSeDebugPrivilege(true);

    bool processIdConvertion = false;

    m_eventName = commandLineParameter(1).toLatin1();
    m_processId = commandLineParameter(2).toInt(&processIdConvertion);

    Q_ASSERT(!m_eventName.isEmpty() && "Event name must be passed!");
    Q_ASSERT(processIdConvertion && "Process ID must be passed!");

    VERIFY(connect(this, &SeoSpiderServiceApp::closeServiceApp, this, &SeoSpiderServiceApp::quit, Qt::QueuedConnection));

    QMetaObject::invokeMethod(this, "waitForSignaledEvent", Qt::QueuedConnection);
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

void SeoSpiderServiceApp::waitForSignaledEvent() const noexcept
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

void SeoSpiderServiceApp::makeCrashDump(HANDLE processHandle) const noexcept
{
    const QString dumpFileName = "crash.dmp";

    HANDLE dumpFileHandle = CreateFileW(dumpFileName.toStdWString().c_str(),
        GENERIC_WRITE, FILE_SHARE_DELETE | FILE_SHARE_READ | FILE_SHARE_WRITE,
        NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);

    MINIDUMP_TYPE miniDumpType = MINIDUMP_TYPE::MiniDumpNormal;

    m_dbgHelpDllLoader->writeDump(processHandle, m_processId, dumpFileHandle, miniDumpType, NULL, NULL, NULL);

    CloseHandle(dumpFileHandle);
}

bool SeoSpiderServiceApp::setSeDebugPrivilege(bool flag) const noexcept
{
    HANDLE hThisProcessToken = nullptr;

    if (OpenProcessToken(GetCurrentProcess(), TOKEN_ADJUST_PRIVILEGES, &hThisProcessToken))
    {
        TOKEN_PRIVILEGES tokenPrivileges;
        DWORD enableStatus = flag ? SE_PRIVILEGE_ENABLED : 0;

        tokenPrivileges.Privileges[0].Attributes = enableStatus;
        tokenPrivileges.PrivilegeCount = 1;

        BOOL bLookupResult = LookupPrivilegeValue(nullptr, SE_DEBUG_NAME, &tokenPrivileges.Privileges[0].Luid);

        if (!bLookupResult)
        {
            CloseHandle(hThisProcessToken);
            return false;
        }

        AdjustTokenPrivileges(hThisProcessToken, FALSE, &tokenPrivileges, sizeof(tokenPrivileges), nullptr, nullptr);
        CloseHandle(hThisProcessToken);
    }

    return GetLastError() == ERROR_SUCCESS;
}

}