#include "stdafx.h"

#include "gtest/gtest.h"
#include "tests_crawler.h"


namespace WebCrawlerTests
{

TEST(DummyGroup, DummyName)
{
	//EXPECT_EQ(0, 0);

	std::unique_ptr<TestsCrawler> crawler = std::make_unique<TestsCrawler>(1);

	WebCrawler::CrawlerOptions options;
	options.host = QUrl("dummy.com");

	crawler->startCrawling(options);
	// TODO: fix: Crawler::startCrawlingInternal has not been called

	std::vector<WebCrawler::ParsedPagePtr> pages = crawler->waitForParsedPageReceived(1, 10);

	EXPECT_EQ(1, pages.size());
}

}

int main(int argc, char* argv[])
{
	testing::InitGoogleTest(&argc, argv);
	return RUN_ALL_TESTS();
}