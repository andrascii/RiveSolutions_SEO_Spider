#pragma once
#include "crawler_options.h"
#include "parsed_page.h"

namespace WebCrawlerTests
{

class TestsCrawler;

class TestEnvironment
{
public:
	TestEnvironment(WebCrawler::CrawlerOptions options);
	TestsCrawler* crawler() const;
	void runTest(std::function<void()> condition) const;

	static WebCrawler::CrawlerOptions defaultOptions(const QUrl& url);

	// helper functions, move to a separate class?
	static WebCrawler::ResourceLink firstResourceOnThisPageOfType(WebCrawler::ParsedPagePtr page, WebCrawler::ResourceType resourceType);
	static WebCrawler::ResourceLink firstResourceToThisPageOfType(WebCrawler::ParsedPagePtr page, WebCrawler::ResourceType resourceType);

private:
	std::unique_ptr<TestsCrawler> m_crawler;
};
}