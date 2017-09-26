#include "stdafx.h"

#include "gtest/gtest.h"
#include "tests_crawler.h"
#include "test_environment.h"


namespace WebCrawlerTests
{

TEST(DummyGroup, DummyName)
{
	TestEnvironment env({ QUrl("dummy.com") });

	const auto condition = [cl = env.crawler()]()
	{
		auto pages = cl->waitForParsedPageReceived(WebCrawler::CrawledUrlStorageType, 1, 10);
		EXPECT_EQ(1, pages.size());
	};

	env.runTest(condition);
}

}

int main(int argc, char* argv[])
{
	testing::InitGoogleTest(&argc, argv);
	return RUN_ALL_TESTS();
}