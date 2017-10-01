#pragma once
#include "crawler_options.h"

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

private:
	std::unique_ptr<TestsCrawler> m_crawler;
};
}