#include "executor.h"
#include "process_watch_dog_thread.h"

int main(int argc, char** argv)
{
	using namespace ScreenshotMaker;

	QApplication app(argc, argv);
	app.setQuitOnLastWindowClosed(false);
	app.setWindowIcon(QIcon(":/images/favicon.ico"));

	const QStringList arguments = app.arguments();

	Q_ASSERT(arguments.size() == 4);

	new Executor(arguments[1], arguments[2], &app);
	ProcessWatchDogThread* watchDog = new ProcessWatchDogThread(arguments[3], &app);

	Q_ASSERT(QObject::connect(watchDog, &ProcessWatchDogThread::processClosed, &app, &QApplication::quit));

	watchDog->start();
	return app.exec();
}