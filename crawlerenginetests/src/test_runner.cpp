#include "test_runner.h"
#include "test_environment.h"

namespace CrawlerEngineTests
{

std::mutex TestRunner::s_mutex;

TestRunner::TestRunner(QObject* parent)
	: QObject(parent)
{
	s_mutex.lock();

	qRegisterMetaType<std::function<void()>>("std::function<void()>");

	//
	// Qt::QueuedConnection important
	//
	VERIFY(connect(testEnv, SIGNAL(testInitialized(const std::function<void()>&)), 
		this, SLOT(runTest(const std::function<void()>&)), Qt::QueuedConnection));
}

void TestRunner::runTest(const std::function<void()>& testFunction)
{
	testEnv->crawler()->startTestCrawler();
	testFunction();

	testEnv->quit();

	s_mutex.unlock();
}

}
