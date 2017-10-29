namespace WebCrawlerTests
{
	
TEST(MetaRobotsTests, TestSeveralTags)
{
	TestEnvironment env(TestEnvironment::defaultOptions(QUrl("http://metarobots.com/index.html")));
	env.runTest([cl = env.crawler()]()
	{
		auto pages = cl->waitForParsedPageReceived(WebCrawler::CrawledUrlStorageType, 1, 10, "Waiting for 1 crawled page");

		EXPECT_EQ(1, pages.size());
		EXPECT_EQ(true, pages[0]->metaRobotsFlags.testFlag(WebCrawler::MetaRobotsAll));
		EXPECT_EQ(true, pages[0]->metaRobotsFlags.testFlag(WebCrawler::MetaRobotsNoIndex));
		EXPECT_EQ(true, pages[0]->metaRobotsFlags.testFlag(WebCrawler::MetaRobotsNoFollow));
		EXPECT_EQ(true, pages[0]->metaRobotsFlags.testFlag(WebCrawler::MetaRobotsNoArchive));
		EXPECT_EQ(true, pages[0]->metaRobotsFlags.testFlag(WebCrawler::MetaRobotsNoSnippet));
		EXPECT_EQ(true, pages[0]->metaRobotsFlags.testFlag(WebCrawler::MetaRobotsNoSnippet));
		EXPECT_EQ(true, pages[0]->metaRobotsFlags.testFlag(WebCrawler::MetaRobotsNoODP));
		EXPECT_EQ(true, pages[0]->metaRobotsFlags.testFlag(WebCrawler::MetaRobotsNoTranslate));
		EXPECT_EQ(true, pages[0]->metaRobotsFlags.testFlag(WebCrawler::MetaRobotsNoImageIndex));		
		EXPECT_EQ(true, pages[0]->metaRobotsFlags.testFlag(WebCrawler::MetaRobotsIndex));
		EXPECT_EQ(true, pages[0]->metaRobotsFlags.testFlag(WebCrawler::MetaRobotsFollow));
		EXPECT_EQ(true, pages[0]->metaRobotsFlags.testFlag(WebCrawler::MetaRobotsNoYaCa));
		EXPECT_EQ(true, pages[0]->metaRobotsFlags.testFlag(WebCrawler::MetaRobotsNoYDir));
	});
}

TEST(MetaRobotsTests, TestXRobotsTag)
{
	TestEnvironment env(TestEnvironment::defaultOptions(QUrl("http://metarobots.com/x-robots-tag.html")));
	env.runTest([cl = env.crawler()]()
	{
		auto pages = cl->waitForParsedPageReceived(WebCrawler::CrawledUrlStorageType, 1, 10, "Waiting for 1 crawled page");

		EXPECT_EQ(1, pages.size());
		EXPECT_EQ(true, pages[0]->metaRobotsFlags.testFlag(WebCrawler::MetaRobotsAll));
		EXPECT_EQ(true, pages[0]->metaRobotsFlags.testFlag(WebCrawler::MetaRobotsNoIndex));
		EXPECT_EQ(true, pages[0]->metaRobotsFlags.testFlag(WebCrawler::MetaRobotsNoFollow));
		EXPECT_EQ(true, pages[0]->metaRobotsFlags.testFlag(WebCrawler::MetaRobotsNoArchive));
		EXPECT_EQ(true, pages[0]->metaRobotsFlags.testFlag(WebCrawler::MetaRobotsNoSnippet));
		EXPECT_EQ(true, pages[0]->metaRobotsFlags.testFlag(WebCrawler::MetaRobotsNoSnippet));
		EXPECT_EQ(true, pages[0]->metaRobotsFlags.testFlag(WebCrawler::MetaRobotsNoODP));
		EXPECT_EQ(true, pages[0]->metaRobotsFlags.testFlag(WebCrawler::MetaRobotsNoTranslate));
		EXPECT_EQ(true, pages[0]->metaRobotsFlags.testFlag(WebCrawler::MetaRobotsNoImageIndex));
		EXPECT_EQ(true, pages[0]->metaRobotsFlags.testFlag(WebCrawler::MetaRobotsIndex));
		EXPECT_EQ(true, pages[0]->metaRobotsFlags.testFlag(WebCrawler::MetaRobotsFollow));
		EXPECT_EQ(true, pages[0]->metaRobotsFlags.testFlag(WebCrawler::MetaRobotsNoYaCa));
		EXPECT_EQ(true, pages[0]->metaRobotsFlags.testFlag(WebCrawler::MetaRobotsNoYDir));
	});
}

}