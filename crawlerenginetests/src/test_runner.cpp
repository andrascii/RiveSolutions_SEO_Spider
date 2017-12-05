#include "test_runner.h"
#include "test_environment.h"

namespace CrawlerEngineTests
{

TestRunner::TestRunner(QObject* parent)
	: QObject(parent)
{
	qRegisterMetaType<std::function<void()>>("std::function<void()>");

	VERIFY(connect(testEnv, SIGNAL(testInitialized(const std::function<void()>&)), 
		this, SLOT(runTest(const std::function<void()>&)), Qt::QueuedConnection));
}

void TestRunner::runTest(const std::function<void()>& testFunction)
{
	QMetaObject::invokeMethod(testEnv->crawler(), "startTestCrawler", Qt::QueuedConnection);

	testFunction();

	testEnv->quit();
}

}
