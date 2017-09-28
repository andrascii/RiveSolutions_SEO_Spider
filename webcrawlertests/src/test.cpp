#include "stdafx.h"

#include "gtest/gtest.h"
#include "tests_crawler.h"
#include "test_environment.h"


namespace WebCrawlerTests
{

TEST(DummyGroup, DummyName)
{
	TestEnvironment env(TestEnvironment::defaultOptions(QUrl("http://dummy.com")));
	env.runTest([cl = env.crawler()]()
	{
		auto pages = cl->waitForParsedPageReceived(WebCrawler::CrawledUrlStorageType, 1, 1000);
		EXPECT_EQ(1, pages.size());
		WebCrawler::ParsedPagePtr page = pages[0];
		EXPECT_EQ(QString("Dummy H1"),  page->firstH1);
		EXPECT_EQ(QString("Dummy Title"), page->title);
		EXPECT_EQ(QString("Dummy Description"), page->metaDescription);
	});
}

}

int main(int argc, char* argv[])
{
	testing::InitGoogleTest(&argc, argv);
	return RUN_ALL_TESTS();
}