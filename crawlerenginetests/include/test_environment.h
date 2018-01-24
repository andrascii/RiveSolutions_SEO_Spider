#pragma once
#include "crawler_options.h"
#include "parsed_page.h"
#include "named_thread.h"

namespace CrawlerEngineTests
{

class TestRunner;
class TestsCrawler;

class TestEnvironment : public QCoreApplication
{
	Q_OBJECT

public:
	TestEnvironment(CrawlerEngine::CrawlerOptions options);
	~TestEnvironment();

	TestsCrawler* crawler() const;
	void initializeTest(const std::function<void()>& testFunction);

	static CrawlerEngine::CrawlerOptions defaultOptions(const CrawlerEngine::Url& url);
	static CrawlerEngine::ResourceLink firstResourceOnThisPageOfType(const CrawlerEngine::ParsedPage* page, CrawlerEngine::ResourceType resourceType);
	static CrawlerEngine::ResourceLink firstResourceToThisPageOfType(const CrawlerEngine::ParsedPage* page, CrawlerEngine::ResourceType resourceType);

signals:
	void testInitialized(const std::function<void()>& testFunction);

private:
	TestRunner* m_testRunner;
	TestsCrawler* m_crawler;
	Common::NamedThread* m_crawlerThread;
	std::function<void()> m_testFunction;
};

}
