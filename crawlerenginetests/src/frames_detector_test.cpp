namespace CrawlerEngineTests
{

using namespace CrawlerEngine;

TEST(FrameTests, FramesDetectorTest)
{
	TestEnvironment env;

	env.crawler()->options()->setData(TestEnvironment::defaultOptions(Url("http://frames.com/index.html")));

	const auto testFunction = [cl = env.crawler()]()
	{
		auto pages = cl->waitForParsedPageReceived(StorageType::CrawledUrlStorageType, 2, 15000, "Waiting for 2 pages");
		auto framePages = cl->waitForParsedPageReceived(StorageType::ContainsFramesStorageType, 1, 15000, "Waiting for 1 page");

		cl->waitForCrawlingDone();

		cl->checkSequencedDataCollectionConsistency();

		EXPECT_EQ(2, pages.size());
		EXPECT_EQ(1, framePages.size());
	};

	env.initializeTest(testFunction);
	env.exec();
}

}