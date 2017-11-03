namespace CrawlerEngineTests
{
	
TEST(MetaRobotsTests, TestSeveralTags)
{
	TestEnvironment env(TestEnvironment::defaultOptions(QUrl("http://metarobots.com/index.html")));
	env.runTest([cl = env.crawler()]()
	{
		auto pages = cl->waitForParsedPageReceived(CrawlerEngine::CrawledUrlStorageType, 1, 10, "Waiting for 1 crawled page");

		EXPECT_EQ(1, pages.size());
		const CrawlerEngine::MetaRobotsFlags flags = pages[0]->metaRobotsFlags[CrawlerEngine::UserAgentType::AnyBot];

		EXPECT_EQ(true, flags.testFlag(CrawlerEngine::MetaRobotsAll));
		EXPECT_EQ(true, flags.testFlag(CrawlerEngine::MetaRobotsNoIndex));
		EXPECT_EQ(true, flags.testFlag(CrawlerEngine::MetaRobotsNoFollow));
		EXPECT_EQ(true, flags.testFlag(CrawlerEngine::MetaRobotsNoArchive));
		EXPECT_EQ(true, flags.testFlag(CrawlerEngine::MetaRobotsNoSnippet));
		EXPECT_EQ(true, flags.testFlag(CrawlerEngine::MetaRobotsNoSnippet));
		EXPECT_EQ(true, flags.testFlag(CrawlerEngine::MetaRobotsNoODP));
		EXPECT_EQ(true, flags.testFlag(CrawlerEngine::MetaRobotsNoTranslate));
		EXPECT_EQ(true, flags.testFlag(CrawlerEngine::MetaRobotsNoImageIndex));
		EXPECT_EQ(true, flags.testFlag(CrawlerEngine::MetaRobotsIndex));
		EXPECT_EQ(true, flags.testFlag(CrawlerEngine::MetaRobotsFollow));
		EXPECT_EQ(true, flags.testFlag(CrawlerEngine::MetaRobotsNoYaCa));
		EXPECT_EQ(true, flags.testFlag(CrawlerEngine::MetaRobotsNoYDir));
	});
}

TEST(MetaRobotsTests, TestXRobotsTag)
{
	TestEnvironment env(TestEnvironment::defaultOptions(QUrl("http://metarobots.com/x-robots-tag.html")));
	env.runTest([cl = env.crawler()]()
	{
		auto pages = cl->waitForParsedPageReceived(CrawlerEngine::CrawledUrlStorageType, 1, 10, "Waiting for 1 crawled page");

		EXPECT_EQ(1, pages.size());
		const CrawlerEngine::MetaRobotsFlags flags = pages[0]->metaRobotsFlags[CrawlerEngine::UserAgentType::AnyBot];

		EXPECT_EQ(true, flags.testFlag(CrawlerEngine::MetaRobotsAll));
		EXPECT_EQ(true, flags.testFlag(CrawlerEngine::MetaRobotsNoIndex));
		EXPECT_EQ(true, flags.testFlag(CrawlerEngine::MetaRobotsNoFollow));
		EXPECT_EQ(true, flags.testFlag(CrawlerEngine::MetaRobotsNoArchive));
		EXPECT_EQ(true, flags.testFlag(CrawlerEngine::MetaRobotsNoSnippet));
		EXPECT_EQ(true, flags.testFlag(CrawlerEngine::MetaRobotsNoSnippet));
		EXPECT_EQ(true, flags.testFlag(CrawlerEngine::MetaRobotsNoODP));
		EXPECT_EQ(true, flags.testFlag(CrawlerEngine::MetaRobotsNoTranslate));
		EXPECT_EQ(true, flags.testFlag(CrawlerEngine::MetaRobotsNoImageIndex));
		EXPECT_EQ(true, flags.testFlag(CrawlerEngine::MetaRobotsIndex));
		EXPECT_EQ(true, flags.testFlag(CrawlerEngine::MetaRobotsFollow));
		EXPECT_EQ(true, flags.testFlag(CrawlerEngine::MetaRobotsNoYaCa));
		EXPECT_EQ(true, flags.testFlag(CrawlerEngine::MetaRobotsNoYDir));
	});
}

}