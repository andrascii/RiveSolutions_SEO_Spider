#include "logger_main_window.h"
#include <Windows.h>

int main(int argc, char *argv[])
{
	//while (!IsDebuggerPresent())
	//{
	//}

	QApplication app(argc, argv);
	LoggerMainWindow w;
	w.show();
	return app.exec();
}