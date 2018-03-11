#pragma once

#include "fatal_error_dialog.h"
#include "debug_help_dll_loader.h"
#include "logger_debug_window.h"
#include "ipc_socket.h"
#include "log_thread.h"

namespace Common
{

class IIpcSignaledObject;

}

class Zippo;

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
	void makeDump(HANDLE processHandle) noexcept;
	void sendReports();
	static QString dumpsPath();
	static QString logFilePath();

	Q_SIGNAL void closeServiceApp() const;
	Q_SLOT void onSendingFinished(const QString& mailId, int result, const QByteArray& log);


private:
	std::unique_ptr<FatalErrorDialog> m_dialog;
	std::unique_ptr<DebugHelpDllLoader> m_dbgHelpDllLoader;

	QByteArray m_eventName;
	DWORD m_processId;

	std::unique_ptr<LoggerDebugWindow> m_loggerDebugWindow;

	HANDLE m_signaledEvent;
	HANDLE m_processHandle;

	int m_timerId;

	std::shared_ptr<Common::IIpcSignaledObject> m_crashEventSignaledObject;

	Zippo* m_zippo;
	mutable std::optional<QDir> m_defferedDeleteDir;

	Common::IpcSocket m_pipeSocket;

	std::unique_ptr<LogThread> m_logThread;
};

}