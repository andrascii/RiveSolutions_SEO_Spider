#include "logger_main_window.h"

int main(int argc, char *argv[])
{
	//while (!IsDebuggerPresent());

	CreateEvent(nullptr, FALSE, FALSE, "QuickieWebBot.Logger.Checking.Process.IsRunning");

	if (GetLastError() == ERROR_ALREADY_EXISTS)
	{
		return ERROR_ALREADY_EXISTS;
	}

	QApplication app(argc, argv);

	LoggerMainWindow w;
	w.show();

	return app.exec();
}