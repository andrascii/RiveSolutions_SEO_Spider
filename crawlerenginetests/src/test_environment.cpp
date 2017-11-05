#include "test_environment.h"
#include "tests_crawler.h"
#include "handler_registry.h"
#include "test_runner.h"

namespace
{

constexpr int s_threadCount = 1;

}

namespace CrawlerEngineTests
{

TestEnvironment::TestEnvironment(int& argc, CrawlerEngine::CrawlerOptions options)
	: QCoreApplication(argc, nullptr)
	, m_testRunner(new TestRunner)
	, m_crawler(new TestsCrawler(s_threadCount, options, nullptr))
{
	m_crawlerThread = new Common::NamedThread("CrawlerTestsThread");
	m_crawlerThread->start();

	m_crawler->moveToThread(m_crawlerThread);
}

TestEnvironment::~TestEnvironment()
{
	Common::HandlerRegistry::instance().unregisterAll();

	m_crawler->deleteLater();
	processEvents();

	m_crawlerThread->quit();
	m_crawlerThread->wait();
}

TestsCrawler* TestEnvironment::crawler() const
{
	return m_crawler;
}

void TestEnvironment::initializeTest(const std::function<void()>& testFunction)
{
	m_testFunction = testFunction;

	emit testInitialized(m_testFunction);
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

CrawlerEngine::ResourceLink TestEnvironment::firstResourceOnThisPageOfType(const CrawlerEngine::ParsedPage* page, CrawlerEngine::ResourceType resourceType)
{
	for (const CrawlerEngine::ResourceLink& link : page->linksOnThisPage)
	{
		if (link.resource.lock()->resourceType == resourceType)
		{
			return link;
		}
	}

	CrawlerEngine::ResourceLink resourceLink
	{
		CrawlerEngine::ParsedPageWeakPtr(),
		CrawlerEngine::LinkParameter::DofollowParameter,
		CrawlerEngine::ResourceSource::SourceInvalid,
		QString::null
	};

	return resourceLink;
}

CrawlerEngine::ResourceLink TestEnvironment::firstResourceToThisPageOfType(const CrawlerEngine::ParsedPage* page, CrawlerEngine::ResourceType resourceType)
{
	for (const CrawlerEngine::ResourceLink& link : page->linksToThisPage)
	{
		if (link.resource.lock()->resourceType == resourceType)
		{
			return link;
		}
	}

	CrawlerEngine::ResourceLink resourceLink
	{
		CrawlerEngine::ParsedPageWeakPtr(),
		CrawlerEngine::LinkParameter::DofollowParameter,
		CrawlerEngine::ResourceSource::SourceInvalid,
		QString::null
	};

	return resourceLink;
}

}
