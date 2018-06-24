#include "executor.h"

int main(int argc, char** argv)
{
	QApplication app(argc, argv);
	app.setQuitOnLastWindowClosed(false);

	const QStringList arguments = app.arguments();

	Q_ASSERT(arguments.size() == 3);

	ScreenshotMaker::Executor executor(arguments[1], arguments[2]);

	return app.exec();
}