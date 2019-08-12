#pragma once
#include "stdafx.h"

#include "fatal_error_dialog.h"
#include "debug_help_dll_loader.h"
#include "logger_debug_window.h"
#include "ipc_socket.h"
#include "command_thread.h"
#include "command.h"

class Zippo;

namespace SeoSpiderService
{

class StatisticsUploader;

class SeoSpiderServiceApp : public QApplication
{
	Q_OBJECT

public:
	SeoSpiderServiceApp(int& argc, char** argv);
	~SeoSpiderServiceApp();


	QString statisticsFilePath();

protected:
	virtual void timerEvent(QTimerEvent*) override;

private:
	void init();

	QString commandLineParameter(int num) const noexcept;
	bool makeDump(HANDLE processHandle, const void* threadId, const void* exceptionInfo, qint64 exceptionSize) noexcept;
	void writeSysInfoFile(const QString& fileName) const;

	Q_SLOT void sendReports();
	static QString dumpsPath();
	static QString logFilePath();

	Q_SIGNAL void closeServiceApp() const;
	Q_SLOT void onServiceClose();

	Q_SLOT void onSendingFinished(const QString& mailId, int result, const QByteArray& log);
	Q_SLOT void onCompressingFinished();
	Q_SLOT void onCommandReceived(Common::Command command);

private:
	std::unique_ptr<FatalErrorDialog> m_dialog;
	std::unique_ptr<DebugHelpDllLoader> m_dbgHelpDllLoader;

	QByteArray m_eventName;
	DWORD m_processId;

	std::unique_ptr<LoggerDebugWindow> m_loggerDebugWindow;
	HANDLE m_processHandle;

	int m_timerId;

	Zippo* m_zippo;
	mutable std::optional<QDir> m_defferedDeleteDir;

	Common::IpcSocket m_pipeSocket;

	std::unique_ptr<CommandThread> m_cmdThread;
	int m_pendingReportsCount;
	bool m_compressionIsActive;

	StatisticsUploader* m_statisticsUploader;

	QString m_statisticsFilePath;
};

}