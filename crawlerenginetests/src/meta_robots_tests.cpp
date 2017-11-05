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

		const auto metaRobotsFlagsIterator = pages[0]->metaRobotsFlags.find(CrawlerEngine::UserAgentType::AnyBot);

		EXPECT_NE(pages[0]->metaRobotsFlags.end(), metaRobotsFlagsIterator);

		const CrawlerEngine::MetaRobotsFlags flags = metaRobotsFlagsIterator->second;

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

		const auto metaRobotsFlagsIterator = pages[0]->metaRobotsFlags.find(CrawlerEngine::UserAgentType::AnyBot);

		EXPECT_NE(pages[0]->metaRobotsFlags.end(), metaRobotsFlagsIterator);

		const CrawlerEngine::MetaRobotsFlags flags = metaRobotsFlagsIterator->second;

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
	};

	env.initializeTest(testFunction);
	env.exec();
}

}