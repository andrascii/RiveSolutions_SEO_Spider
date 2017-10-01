namespace WebCrawlerTests
{

TEST(TitleTests, EmptyTitle)
{
	WebCrawlerTests::TestEnvironment env({ QUrl("http://title.com/empty-title.html") });
	env.runTest([cl = env.crawler()]()
	{
		auto pages = cl->waitForParsedPageReceived(WebCrawler::EmptyTitleUrlStorageType, 1, 10);
		EXPECT_EQ(1, pages.size());
		EXPECT_EQ(QString(""), pages[0]->title);
	});
}


TEST(TitleTests, NoTitle)
{
	WebCrawlerTests::TestEnvironment env({ QUrl("http://title.com/no-title.html") });
	env.runTest([cl = env.crawler()]()
	{
		auto pages = cl->waitForParsedPageReceived(WebCrawler::EmptyTitleUrlStorageType, 1, 10);
		EXPECT_EQ(1, pages.size());
		EXPECT_EQ(QString(""), pages[0]->title);
	});
}


TEST(TitleTests, TooLongTitle)
{
	WebCrawler::CrawlerOptions options = TestEnvironment::defaultOptions(QUrl("http://title.com/too-long-title.html"));
	options.maxTitleLength = 10;
	WebCrawlerTests::TestEnvironment env(options);
	env.runTest([cl = env.crawler()]()
	{
		auto pages = cl->waitForParsedPageReceived(WebCrawler::VeryLongTitleUrlStorageType, 1, 10);
		EXPECT_EQ(1, pages.size());
		EXPECT_EQ(QString("This is too long title"), pages[0]->title);
	});
}

//

TEST(TitleTests, TooShortTitle)
{
	WebCrawler::CrawlerOptions options = TestEnvironment::defaultOptions(QUrl("http://title.com/too-short-title.html"));
	options.minTitleLength = 24;
	options.maxTitleLength = 100;
	WebCrawlerTests::TestEnvironment env(options);
	env.runTest([cl = env.crawler()]()
	{
		auto pages = cl->waitForParsedPageReceived(WebCrawler::VeryShortTitleUrlStorageType, 1, 10);
		EXPECT_EQ(1, pages.size());
		EXPECT_EQ(QString("This is too short title"), pages[0]->title);
	});
}

}
