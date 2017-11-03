#include "test_environment.h"
#include "tests_crawler.h"
#include "handler_registry.h"

namespace CrawlerEngineTests
{

TestEnvironment::TestEnvironment(CrawlerEngine::CrawlerOptions options)
	: m_crawler(std::make_unique<TestsCrawler>(1, options))
{
}

TestEnvironment::~TestEnvironment()
{
	Common::HandlerRegistry::instance().unregisterAll();
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

CrawlerEngine::CrawlerOptions TestEnvironment::defaultOptions(const QUrl& url)
{
	CrawlerEngine::CrawlerOptions result{ url };
	result.parserTypeFlags = CrawlerEngine::ParserTypeFlags(
		CrawlerEngine::JavaScriptResourcesParserType |
		CrawlerEngine::CssResourcesParserType |
		CrawlerEngine::ImagesResourcesParserType |
		CrawlerEngine::VideoResourcesParserType |
		CrawlerEngine::FlashResourcesParserType);

	result.minTitleLength = 0;
	result.maxTitleLength = 1000;

	return result;
}

CrawlerEngine::ResourceLink TestEnvironment::firstResourceOnThisPageOfType(CrawlerEngine::ParsedPagePtr page, CrawlerEngine::ResourceType resourceType)
{
	for (CrawlerEngine::ResourceLink& link : page->linksOnThisPage)
	{
		if (link.resource.lock()->resourceType == resourceType)
		{
			return link;
		}
	}

	return CrawlerEngine::ResourceLink
	{ 
		CrawlerEngine::ParsedPageWeakPtr(), 
		CrawlerEngine::LinkParameter::DofollowParameter,
		CrawlerEngine::ResourceSource::SourceInvalid, 
		QString() 
	};
}

CrawlerEngine::ResourceLink TestEnvironment::firstResourceToThisPageOfType(CrawlerEngine::ParsedPagePtr page, CrawlerEngine::ResourceType resourceType)
{
	for (CrawlerEngine::ResourceLink& link : page->linksToThisPage)
	{
		if (link.resource.lock()->resourceType == resourceType)
		{
			return link;
		}
	}

	return CrawlerEngine::ResourceLink
	{
		CrawlerEngine::ParsedPageWeakPtr(),
		CrawlerEngine::LinkParameter::DofollowParameter,
		CrawlerEngine::ResourceSource::SourceInvalid,
		QString()
	};
}

}
