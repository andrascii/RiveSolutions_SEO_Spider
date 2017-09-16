#pragma once

#include "fatal_error_dialog.h"
#include "debug_help_dll_loader.h"

namespace WatchDogService
{

class WatchDogServiceApp : public QApplication
{
	Q_OBJECT

public:
	WatchDogServiceApp(int& argc, char** argv);

	Q_SLOT void waitForSignaledEvent() const noexcept;

private:
	QString commandLineParameter(int num) const noexcept;

	void makeCrashDump(HANDLE processHandle) const noexcept;
	bool setSeDebugPrivilege(bool flag) const noexcept;

	Q_SIGNAL void closeServiceApp() const;

private:
	std::unique_ptr<FatalErrorDialog> m_dialog;
	std::unique_ptr<DebugHelpDllLoader> m_dbgHelpDllLoader;

	QByteArray m_eventName;
	DWORD m_processId;
};

}
