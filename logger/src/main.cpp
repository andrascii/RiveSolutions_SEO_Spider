#include "logger_main_window.h"

#ifndef Q_OS_WIN

#define ERROR_ALREADY_EXISTS 183L

#endif

namespace
{

void doCheckingClones()
{
#ifdef Q_OS_WIN

	CreateEvent(nullptr, FALSE, FALSE, "QuickieWebBot.Logger.Checking.Process.IsRunning");

	if (GetLastError() == ERROR_ALREADY_EXISTS)
	{
		throw ERROR_ALREADY_EXISTS;
	}

#endif
}

}

int main(int argc, char *argv[])
{
	try
	{
		doCheckingClones();

		QApplication app(argc, argv);

		LoggerMainWindow w;
		w.show();

		return app.exec();
	}
	catch (...)
	{
		return ERROR_ALREADY_EXISTS;
	}
}