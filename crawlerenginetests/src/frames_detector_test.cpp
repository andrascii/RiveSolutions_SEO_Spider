#include "stdafx.h"
namespace CrawlerEngineTests
{

using namespace CrawlerEngine;

TEST(FrameTests, FramesDetectorTest)
{
	TestEnvironment env;

	env.crawler()->options()->setData(TestEnvironment::defaultOptions(Url("http://frames.com/index.html")));

	const auto testFunction = [cl = env.crawler()]()
	{
		cl->waitForCrawlingDone();

		auto pages = cl->storageItems(StorageType::CrawledUrlStorageType);
		auto framePages = cl->storageItems(StorageType::ContainsFramesStorageType);

		cl->checkSequencedDataCollectionConsistency();

		EXPECT_EQ(2, pages.size());
		EXPECT_EQ(1, framePages.size());
	};

	env.initializeTest(testFunction);
	env.exec();
}

}