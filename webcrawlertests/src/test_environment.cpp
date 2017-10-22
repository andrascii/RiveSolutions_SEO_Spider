#include "test_environment.h"
#include "tests_crawler.h"
#include "named_thread.h"

namespace WebCrawlerTests
{

TestEnvironment::TestEnvironment(WebCrawler::CrawlerOptions options)
{
	m_crawler = std::make_unique<TestsCrawler>(1, options);

	m_crawlerThread = new Common::NamedThread("Crawler");
	m_crawler->moveToThread(m_crawlerThread);
	m_crawlerThread->start();
}


TestEnvironment::~TestEnvironment()
{
	ASSERT(QThread::currentThread() != m_crawlerThread)
	m_crawlerThread->quit();
	m_crawlerThread->wait();
	m_crawlerThread->deleteLater();
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

WebCrawler::CrawlerOptions TestEnvironment::defaultOptions(const QUrl& url)
{
	WebCrawler::CrawlerOptions result{ url };
	result.parserTypeFlags = WebCrawler::ParserTypeFlags(
		WebCrawler::JavaScriptResourcesParserType |
		WebCrawler::CssResourcesParserType |
		WebCrawler::ImagesResourcesParserType |
		WebCrawler::VideoResourcesParserType |
		WebCrawler::FlashResourcesParserType);

	result.minTitleLength = 0;
	result.maxTitleLength = 1000;

	return result;
}

WebCrawler::ResourceLink TestEnvironment::firstResourceOnThisPageOfType(WebCrawler::ParsedPagePtr page, WebCrawler::ResourceType resourceType)
{
	for (WebCrawler::ResourceLink& link : page->linksOnThisPage)
	{
		if (link.resource.lock()->resourceType == resourceType)
		{
			return link;
		}
	}

	return WebCrawler::ResourceLink
	{ 
		WebCrawler::ParsedPageWeakPtr(), 
		WebCrawler::LinkParameter::DofollowParameter,
		WebCrawler::ResourceSource::SourceInvalid, 
		QString() 
	};
}

WebCrawler::ResourceLink TestEnvironment::firstResourceToThisPageOfType(WebCrawler::ParsedPagePtr page, WebCrawler::ResourceType resourceType)
{
	for (WebCrawler::ResourceLink& link : page->linksToThisPage)
	{
		if (link.resource.lock()->resourceType == resourceType)
		{
			return link;
		}
	}

	return WebCrawler::ResourceLink
	{
		WebCrawler::ParsedPageWeakPtr(),
		WebCrawler::LinkParameter::DofollowParameter,
		WebCrawler::ResourceSource::SourceInvalid,
		QString()
	};
}

}
