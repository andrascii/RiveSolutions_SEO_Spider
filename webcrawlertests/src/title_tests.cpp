namespace WebCrawlerTets
{

TEST(TitleTests, DummyName)
{
	WebCrawlerTests::TestEnvironment env({ QUrl("dummy.com") });
	env.runTest([cl = env.crawler()]()
	{
		auto pages = cl->waitForParsedPageReceived(WebCrawler::CrawledUrlStorageType, 1, 10);
		EXPECT_EQ(1, pages.size());
	});
}

}
