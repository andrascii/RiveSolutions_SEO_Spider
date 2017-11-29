#pragma once

#include "fatal_error_dialog.h"
#include "debug_help_dll_loader.h"
#include "logger_debug_window.h"

namespace SeoSpiderService
{

class SeoSpiderServiceApp : public QApplication
{
    Q_OBJECT

public:
    SeoSpiderServiceApp(int& argc, char** argv);
    ~SeoSpiderServiceApp();

protected:
    virtual void timerEvent(QTimerEvent*) override;

private:
    void init();

    QString commandLineParameter(int num) const noexcept;

    void makeDump(HANDLE processHandle) const noexcept;

    Q_SIGNAL void closeServiceApp() const;

private:
    std::unique_ptr<FatalErrorDialog> m_dialog;
    std::unique_ptr<DebugHelpDllLoader> m_dbgHelpDllLoader;

    QByteArray m_eventName;
    DWORD m_processId;

    std::unique_ptr<LoggerDebugWindow> m_loggerDebugWindow;

    HANDLE m_signaledEvent;
    HANDLE m_processHandle;

    int m_timerId;
};

}