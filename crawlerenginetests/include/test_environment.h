#pragma once
#include "crawler_options.h"
#include "parsed_page.h"
#include "named_thread.h"

namespace CrawlerEngineTests
{

class TestsCrawler;

class TestEnvironment
{
public:
	TestEnvironment(CrawlerEngine::CrawlerOptions options);
	~TestEnvironment();
	TestsCrawler* crawler() const;
	void runTest(std::function<void()> condition) const;

	static CrawlerEngine::CrawlerOptions defaultOptions(const QUrl& url);

	// helper functions, move to a separate class?
	static CrawlerEngine::ResourceLink firstResourceOnThisPageOfType(CrawlerEngine::ParsedPagePtr page, CrawlerEngine::ResourceType resourceType);
	static CrawlerEngine::ResourceLink firstResourceToThisPageOfType(CrawlerEngine::ParsedPagePtr page, CrawlerEngine::ResourceType resourceType);

private:
	std::unique_ptr<TestsCrawler> m_crawler;
	Common::NamedThread* m_crawlerThread;
};

}
