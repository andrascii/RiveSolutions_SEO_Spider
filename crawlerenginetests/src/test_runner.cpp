#include "test_runner.h"
#include "test_environment.h"

namespace
{

class TestRunnerThread : public QThread
{
public:
	TestRunnerThread(const std::function<void()>& f, QObject* parent = nullptr)
		: QThread(parent)
		, m_f(f)
	{
	}

protected:
	virtual void run() override
	{
		m_f();
	}

private:
	std::function<void()> m_f;
};

}

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
	QEventLoop eventLoop;

	VERIFY(QMetaObject::invokeMethod(testEnv->crawler(), "startCrawling", Qt::QueuedConnection));

	QThread* thread = new TestRunnerThread(testFunction, this);

	VERIFY(connect(thread, &QThread::finished, &eventLoop, &QEventLoop::quit, Qt::QueuedConnection));

	thread->start();

	eventLoop.exec();

	testEnv->quit();
}

void TestRunner::onTestFinished()
{
	testEnv->quit();
}

}
