#include "logger_main_window.h"

int main(int argc, char *argv[])
{
	QApplication app(argc, argv);
	LoggerMainWindow w;
	w.show();
	return app.exec();
}