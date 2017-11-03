namespace CrawlerEngineTests
{
	
TEST(MetaRobotsTests, TestSeveralTags)
{
	int argc = 0;
	TestEnvironment env(argc, TestEnvironment::defaultOptions(QUrl("http://metarobots.com/index.html")));

	const auto testFunction = [cl = env.crawler()]()
	{
		auto pages = cl->waitForParsedPageReceived(CrawlerEngine::CrawledUrlStorageType, 1, 10, "Waiting for 1 crawled page");

		EXPECT_EQ(1, pages.size());
		EXPECT_EQ(true, pages[0]->metaRobotsFlags.testFlag(CrawlerEngine::MetaRobotsAll));
		EXPECT_EQ(true, pages[0]->metaRobotsFlags.testFlag(CrawlerEngine::MetaRobotsNoIndex));
		EXPECT_EQ(true, pages[0]->metaRobotsFlags.testFlag(CrawlerEngine::MetaRobotsNoFollow));
		EXPECT_EQ(true, pages[0]->metaRobotsFlags.testFlag(CrawlerEngine::MetaRobotsNoArchive));
		EXPECT_EQ(true, pages[0]->metaRobotsFlags.testFlag(CrawlerEngine::MetaRobotsNoSnippet));
		EXPECT_EQ(true, pages[0]->metaRobotsFlags.testFlag(CrawlerEngine::MetaRobotsNoSnippet));
		EXPECT_EQ(true, pages[0]->metaRobotsFlags.testFlag(CrawlerEngine::MetaRobotsNoODP));
		EXPECT_EQ(true, pages[0]->metaRobotsFlags.testFlag(CrawlerEngine::MetaRobotsNoTranslate));
		EXPECT_EQ(true, pages[0]->metaRobotsFlags.testFlag(CrawlerEngine::MetaRobotsNoImageIndex));
		EXPECT_EQ(true, pages[0]->metaRobotsFlags.testFlag(CrawlerEngine::MetaRobotsIndex));
		EXPECT_EQ(true, pages[0]->metaRobotsFlags.testFlag(CrawlerEngine::MetaRobotsFollow));
		EXPECT_EQ(true, pages[0]->metaRobotsFlags.testFlag(CrawlerEngine::MetaRobotsNoYaCa));
		EXPECT_EQ(true, pages[0]->metaRobotsFlags.testFlag(CrawlerEngine::MetaRobotsNoYDir));
	};

	env.initializeTest(testFunction);
	env.exec();
}

TEST(MetaRobotsTests, TestXRobotsTag)
{
	int argc = 0;
	TestEnvironment env(argc, TestEnvironment::defaultOptions(QUrl("http://metarobots.com/x-robots-tag.html")));

	const auto testFunction = [cl = env.crawler()]()
	{
		auto pages = cl->waitForParsedPageReceived(CrawlerEngine::CrawledUrlStorageType, 1, 10, "Waiting for 1 crawled page");

		EXPECT_EQ(1, pages.size());
		EXPECT_EQ(true, pages[0]->metaRobotsFlags.testFlag(CrawlerEngine::MetaRobotsAll));
		EXPECT_EQ(true, pages[0]->metaRobotsFlags.testFlag(CrawlerEngine::MetaRobotsNoIndex));
		EXPECT_EQ(true, pages[0]->metaRobotsFlags.testFlag(CrawlerEngine::MetaRobotsNoFollow));
		EXPECT_EQ(true, pages[0]->metaRobotsFlags.testFlag(CrawlerEngine::MetaRobotsNoArchive));
		EXPECT_EQ(true, pages[0]->metaRobotsFlags.testFlag(CrawlerEngine::MetaRobotsNoSnippet));
		EXPECT_EQ(true, pages[0]->metaRobotsFlags.testFlag(CrawlerEngine::MetaRobotsNoSnippet));
		EXPECT_EQ(true, pages[0]->metaRobotsFlags.testFlag(CrawlerEngine::MetaRobotsNoODP));
		EXPECT_EQ(true, pages[0]->metaRobotsFlags.testFlag(CrawlerEngine::MetaRobotsNoTranslate));
		EXPECT_EQ(true, pages[0]->metaRobotsFlags.testFlag(CrawlerEngine::MetaRobotsNoImageIndex));
		EXPECT_EQ(true, pages[0]->metaRobotsFlags.testFlag(CrawlerEngine::MetaRobotsIndex));
		EXPECT_EQ(true, pages[0]->metaRobotsFlags.testFlag(CrawlerEngine::MetaRobotsFollow));
		EXPECT_EQ(true, pages[0]->metaRobotsFlags.testFlag(CrawlerEngine::MetaRobotsNoYaCa));
		EXPECT_EQ(true, pages[0]->metaRobotsFlags.testFlag(CrawlerEngine::MetaRobotsNoYDir));
	};

	env.initializeTest(testFunction);
	env.exec();
}

}