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

static int s_argc = 0;

TestEnvironment::TestEnvironment(CrawlerEngine::CrawlerOptions options)
	: QCoreApplication(s_argc, nullptr)
	, m_testRunner(new TestRunner)
	, m_crawler(new TestsCrawler(s_threadCount, options, nullptr))
{
	m_crawlerThread = new Common::NamedThread("CrawlerTestsThread");
	m_crawler->moveToThread(m_crawlerThread);
	m_crawlerThread->start();

	QMetaObject::invokeMethod(m_crawler, "initialize", Qt::BlockingQueuedConnection);
}

TestEnvironment::~TestEnvironment()
{
	m_crawler->deleteLater();
	processEvents();

	m_crawlerThread->quit();
	m_crawlerThread->wait();

	CrawlerEngine::HandlerRegistry::instance().unregisterAll();
}

TestsCrawler* TestEnvironment::crawler() const
{
	return m_crawler;
}

void TestEnvironment::initializeTest(const std::function<void()>& testFunction)
{
	CrawlerSharedState* state = CrawlerSharedState::instance();
	state->setDownloaderCrawledLinksCount(0);
	state->setDownloaderPendingLinksCount(0);
	state->setWorkersProcessedLinksCount(0);
	state->setModelControllerAcceptedLinksCount(0);
	state->setModelControllerCrawledLinksCount(0);
	state->setSequencedDataCollectionLinksCount(0);

	m_testFunction = testFunction;

	emit testInitialized(m_testFunction);
}

CrawlerEngine::CrawlerOptions TestEnvironment::defaultOptions(const CustomUrl& url)
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

	return CrawlerEngine::ResourceLink
	{
		CrawlerEngine::ParsedPageWeakPtr(),
		QUrl(),
		CrawlerEngine::LinkParameter::DofollowParameter,
		CrawlerEngine::ResourceSource::SourceInvalid,
		QString::null
	};
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

	return CrawlerEngine::ResourceLink
	{
		CrawlerEngine::ParsedPageWeakPtr(),
		QUrl(),
		CrawlerEngine::LinkParameter::DofollowParameter,
		CrawlerEngine::ResourceSource::SourceInvalid,
		QString::null
	};
}

}
