#include "test_data_feed.h"

namespace CrawlerEngineTests
{

TEST(DataFeedTests, TestDataFeed)
{
	TestEnvironment env;
	env.crawler()->options()->setData(TestEnvironment::defaultOptions(Url("http://links.com/datafeed.com/index.html")));
	env.crawler()->addCustomDataFeed(new TestDataFeed());

	const auto testFunction = [cl = env.crawler()]()
	{
		auto pages = cl->waitForAllCrawledPageReceived(5);
		EXPECT_EQ(1, pages.size());
		
		const auto dataFeedsData = pages.at(0)->dataFeedsData;
		EXPECT_EQ(true, dataFeedsData.contains(DataFeedId::TestDataFeed));

		EXPECT_EQ(QString("Value1"), dataFeedsData[DataFeedId::TestDataFeed][0]);
		EXPECT_EQ(QString("Value2"), dataFeedsData[DataFeedId::TestDataFeed][1]);

		ICustomDataFeed* dataFeed = cl->customDataFeedByName("Test Data Feed");
		EXPECT_EQ(true, dataFeed != nullptr);

		EXPECT_EQ(QString("Column1"), dataFeed->columns()[0]);
		EXPECT_EQ(QString("Column2"), dataFeed->columns()[1]);
	};

	env.initializeTest(testFunction);
	env.exec();
}

}
