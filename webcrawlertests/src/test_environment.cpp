#include "test_environment.h"
#include "tests_crawler.h"

namespace WebCrawlerTests
{

TestEnvironment::TestEnvironment(const WebCrawler::CrawlerOptions& options)
	: m_crawler(std::make_unique<TestsCrawler>(1, options))
{
}


TestsCrawler* TestEnvironment::crawler() const
{
	return m_crawler.get();
}

void TestEnvironment::runTest(std::function<void()> condition) const
{
	int argc = 0;
	QCoreApplication app(argc, nullptr);

	auto extCond = [&app, &condition]
	{
		condition();
		app.quit();
	};

	crawler()->setCondition(extCond);
	QTimer::singleShot(0, crawler(), SLOT(startTestCrawler()));
	app.exec();
	
}

}