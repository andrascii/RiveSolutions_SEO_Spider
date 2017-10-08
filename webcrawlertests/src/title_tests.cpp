namespace WebCrawlerTests
{

TEST(TitleTests, EmptyTitle)
{
	// empty-title.html -> empty-title-2.html
	TestEnvironment env(TestEnvironment::defaultOptions({ QUrl("http://title.com/empty-title.html") }));
	env.runTest([cl = env.crawler()]()
	{
		auto pages = cl->waitForParsedPageReceived(WebCrawler::EmptyTitleUrlStorageType, 2, 10, "Waiting for 2 empty title pages");
		EXPECT_EQ(2, pages.size());
		EXPECT_EQ(QString(""), pages[0]->title);
		EXPECT_EQ(QString(""), pages[1]->title);
		cl->waitForParsedPageReceived(WebCrawler::CrawledUrlStorageType, 2, 10, "Waiting for 2 crawled pages");
	});
}


TEST(TitleTests, NoTitle)
{
	// no-title.html -> no-title-2.html
	TestEnvironment env(TestEnvironment::defaultOptions({ QUrl("http://title.com/no-title.html") }));
	env.runTest([cl = env.crawler()]()
	{
		auto pages = cl->waitForParsedPageReceived(WebCrawler::EmptyTitleUrlStorageType, 2, 10, "Waiting for 2 empty title pages");
		EXPECT_EQ(2, pages.size());
		EXPECT_EQ(QString(""), pages[0]->title);
		EXPECT_EQ(QString(""), pages[1]->title);
		cl->waitForParsedPageReceived(WebCrawler::CrawledUrlStorageType, 2, 10, "Waiting for 2 crawled pages");
	});
}


TEST(TitleTests, TooLongTitle)
{
	// too-long-title.html -> too-long-title-2.html
	WebCrawler::CrawlerOptions options = TestEnvironment::defaultOptions(QUrl("http://title.com/too-long-title.html"));
	options.maxTitleLength = 10;
	TestEnvironment env(options);
	env.runTest([cl = env.crawler()]()
	{
		auto pages = cl->waitForParsedPageReceived(WebCrawler::VeryLongTitleUrlStorageType, 2, 10, "Waiting for 2 too long title pages");
		EXPECT_EQ(2, pages.size());
		EXPECT_EQ(QString("This is too long title"), pages[0]->title);
		EXPECT_EQ(QString("This is too long title"), pages[1]->title);
		cl->waitForParsedPageReceived(WebCrawler::CrawledUrlStorageType, 2, 10, "Waiting for 2 crawled pages");
	});
}

TEST(TitleTests, TooShortTitle)
{
	// too-short-title.html -> too-short-title-2.html
	WebCrawler::CrawlerOptions options = TestEnvironment::defaultOptions(QUrl("http://title.com/too-short-title.html"));
	options.minTitleLength = 24;
	options.maxTitleLength = 100;
	TestEnvironment env(options);
	env.runTest([cl = env.crawler()]()
	{
		auto pages = cl->waitForParsedPageReceived(WebCrawler::VeryShortTitleUrlStorageType, 2, 10, "Waiting for 2 too short title pages");
		EXPECT_EQ(2, pages.size());
		EXPECT_EQ(QString("This is too short title"), pages[0]->title);
		EXPECT_EQ(QString("This is too short title"), pages[1]->title);
		cl->waitForParsedPageReceived(WebCrawler::CrawledUrlStorageType, 2, 10, "Waiting for 2 crawled pages");
	});
}

TEST(TitleTests, DuplicatedTitles)
{
	// duplicated-titles-1.html -> duplicated-titles-2.html
	WebCrawler::CrawlerOptions options = TestEnvironment::defaultOptions(QUrl("http://title.com/duplicated-titles-1.html"));
	options.minTitleLength = 24;
	options.maxTitleLength = 100;
	TestEnvironment env(options);
	env.runTest([cl = env.crawler()]()
	{
		auto pages = cl->waitForParsedPageReceived(WebCrawler::DuplicatedTitleUrlStorageType, 2, 10, "Waiting for 2 duplicated title pages");
		EXPECT_EQ(2, pages.size());
		EXPECT_EQ(QString("Duplicated Title"), pages[0]->title);
		EXPECT_EQ(QString("Duplicated Title"), pages[1]->title);
		cl->waitForParsedPageReceived(WebCrawler::CrawledUrlStorageType, 2, 10, "Waiting for 2 crawled pages");
		cl->waitForParsedPageReceived(WebCrawler::VeryShortTitleUrlStorageType, 2, 10, "Waiting for 2 too short title pages");
	});
}

TEST(TitleTests, DuplicatedH1Title)
{
	// duplicated-h1-title.html -> duplicated-h1-title-2.html
	TestEnvironment env(TestEnvironment::defaultOptions(QUrl("http://title.com/duplicated-h1-title.html")));
	env.runTest([cl = env.crawler()]()
	{
		auto pages = cl->waitForParsedPageReceived(WebCrawler::DuplicatedH1TitleUrlStorageType, 2, 10, "Waiting for 2 duplicated h1 title pages");
		EXPECT_EQ(2, pages.size());
		EXPECT_EQ(QString("Duplicated H1 Title"), pages[0]->title);
		EXPECT_EQ(QString("Duplicated H1 Title"), pages[1]->title);

		EXPECT_EQ(QString("Duplicated H1 Title"), pages[0]->firstH1);
		EXPECT_EQ(QString("Duplicated H1 Title"), pages[1]->firstH1);

		cl->waitForParsedPageReceived(WebCrawler::CrawledUrlStorageType, 2, 10, "Waiting for 2 crawled pages");
	});
}

TEST(TitleTests, SeveralTitles)
{
	// several-titles.html -> several-titles-2.html
	TestEnvironment env(TestEnvironment::defaultOptions(QUrl("http://title.com/several-titles.html")));
	env.runTest([cl = env.crawler()]()
	{
		auto pages = cl->waitForParsedPageReceived(WebCrawler::SeveralTitleUrlStorageType, 2, 10, "Waiting for 2 several titles pages");
		EXPECT_EQ(2, pages.size());
		EXPECT_EQ(QString("First Title"), pages[0]->title);
		EXPECT_EQ(QString("First Title"), pages[1]->title);
		cl->waitForParsedPageReceived(WebCrawler::CrawledUrlStorageType, 2, 10, "Waiting for 2 crawled pages");
	});
}

}
