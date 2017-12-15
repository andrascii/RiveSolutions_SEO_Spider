namespace CrawlerEngineTests
{

TEST(TitleTests, EmptyTitle)
{
	// empty-title.html -> empty-title-2.html
	TestEnvironment env(TestEnvironment::defaultOptions({ CustomUrl("http://title.com/empty-title.html") }));

	const auto testFunction = [cl = env.crawler()]()
	{
		auto pages = cl->waitForParsedPageReceived(CrawlerEngine::EmptyTitleUrlStorageType, 2, 10, "Waiting for 2 empty title pages");
		EXPECT_EQ(2, pages.size());
		EXPECT_EQ(QString::null, pages[0]->title);
		EXPECT_EQ(QString::null, pages[1]->title);
		cl->waitForParsedPageReceived(CrawlerEngine::CrawledUrlStorageType, 2, 10, "Waiting for 2 crawled pages");
	};

	env.initializeTest(testFunction);
	env.exec();
}


TEST(TitleTests, NoTitle)
{
	// no-title.html -> no-title-2.html
	TestEnvironment env(TestEnvironment::defaultOptions({ CustomUrl("http://title.com/no-title.html") }));

	const auto testFunction = [cl = env.crawler()]()
	{
		auto pages = cl->waitForParsedPageReceived(CrawlerEngine::EmptyTitleUrlStorageType, 2, 10, "Waiting for 2 empty title pages");
		EXPECT_EQ(2, pages.size());
		EXPECT_EQ(QString::null, pages[0]->title);
		EXPECT_EQ(QString::null, pages[1]->title);
		cl->waitForParsedPageReceived(CrawlerEngine::CrawledUrlStorageType, 2, 10, "Waiting for 2 crawled pages");
	};

	env.initializeTest(testFunction);
	env.exec();
}


TEST(TitleTests, TooLongTitle)
{
	// too-long-title.html -> too-long-title-2.html
	CrawlerEngine::CrawlerOptions options = TestEnvironment::defaultOptions(CustomUrl("http://title.com/too-long-title.html"));
	options.maxTitleLength = 10;
	TestEnvironment env(options);

	const auto testFunction = [cl = env.crawler()]()
	{
		auto pages = cl->waitForParsedPageReceived(CrawlerEngine::VeryLongTitleUrlStorageType, 2, 10, "Waiting for 2 too long title pages");
		EXPECT_EQ(2, pages.size());
		EXPECT_EQ(QString("This is too long title"), pages[0]->title);
		EXPECT_EQ(QString("This is too long title"), pages[1]->title);
		cl->waitForParsedPageReceived(CrawlerEngine::CrawledUrlStorageType, 2, 10, "Waiting for 2 crawled pages");
	};

	env.initializeTest(testFunction);
	env.exec();
}

TEST(TitleTests, TooShortTitle)
{
	// too-short-title.html -> too-short-title-2.html
	CrawlerEngine::CrawlerOptions options = TestEnvironment::defaultOptions(CustomUrl("http://title.com/too-short-title.html"));
	options.minTitleLength = 24;
	options.maxTitleLength = 100;
	TestEnvironment env(options);

	const auto testFunction = [cl = env.crawler()]()
	{
		auto pages = cl->waitForParsedPageReceived(CrawlerEngine::VeryShortTitleUrlStorageType, 2, 10, "Waiting for 2 too short title pages");
		EXPECT_EQ(2, pages.size());
		EXPECT_EQ(QString("This is too short title"), pages[0]->title);
		EXPECT_EQ(QString("This is too short title"), pages[1]->title);
		cl->waitForParsedPageReceived(CrawlerEngine::CrawledUrlStorageType, 2, 10, "Waiting for 2 crawled pages");
	};

	env.initializeTest(testFunction);
	env.exec();
}

TEST(TitleTests, DuplicatedTitles)
{
	// duplicated-titles-1.html -> duplicated-titles-2.html

	//std::lock_guard<std::mutex> locker(g_mutex);

	CrawlerEngine::CrawlerOptions options = TestEnvironment::defaultOptions(CustomUrl("http://title.com/duplicated-titles-1.html"));
	options.minTitleLength = 24;
	options.maxTitleLength = 100;
	TestEnvironment env(options);

	const auto testFunction = [cl = env.crawler()]()
	{
		cl->waitForAllCrawledPageReceived(10);
		auto pages = cl->storageItems(CrawlerEngine::DuplicatedTitleUrlStorageType);
		EXPECT_EQ(2, pages.size());
		EXPECT_EQ(QString("Duplicated Title"), pages[0]->title);
		EXPECT_EQ(QString("Duplicated Title"), pages[1]->title);
		cl->waitForParsedPageReceived(CrawlerEngine::CrawledUrlStorageType, 2, 10, "Waiting for 2 crawled pages");
		cl->waitForParsedPageReceived(CrawlerEngine::VeryShortTitleUrlStorageType, 2, 10, "Waiting for 2 too short title pages");
	};

	env.initializeTest(testFunction);
	env.exec();
}

TEST(TitleTests, DoNotIncludeCanonicalDuplicatedTitles)
{
	// canonical-duplicated-title.html -> canonical-duplicated-title.html

	CrawlerEngine::CrawlerOptions options = TestEnvironment::defaultOptions(CustomUrl("http://title.com/canonical-duplicated-title.html"));
	TestEnvironment env(options);

	const auto testFunction = [cl = env.crawler()]()
	{
		cl->waitForAllCrawledPageReceived(10);
		auto pages = cl->storageItems(CrawlerEngine::DuplicatedTitleUrlStorageType);
		EXPECT_EQ(0, pages.size());
	};

	env.initializeTest(testFunction);
	env.exec();
}

TEST(TitleTests, IncludeDuplicatedTitlesIfThereAreSeveralCanonical)
{
	// canonical-duplicated-title-another.html -> canonical-duplicated-title.html -> canonical-duplicated-title.html

	CrawlerEngine::CrawlerOptions options = TestEnvironment::defaultOptions(CustomUrl("http://title.com/canonical-duplicated-title-another.html"));
	TestEnvironment env(options);

	const auto testFunction = [cl = env.crawler()]()
	{
		cl->waitForAllCrawledPageReceived(10);
		auto pages = cl->storageItems(CrawlerEngine::DuplicatedTitleUrlStorageType);
		EXPECT_EQ(3, pages.size());
	};

	env.initializeTest(testFunction);
	env.exec();
}

TEST(TitleTests, DoNotIncludeEveryPageInDuplicates)
{
	CrawlerEngine::CrawlerOptions options = TestEnvironment::defaultOptions(CustomUrl("http://title.com/single-page.html"));
	TestEnvironment env(options);

	const auto testFunction = [cl = env.crawler()]()
	{
		cl->waitForAllCrawledPageReceived(10);
		auto pages = cl->storageItems(CrawlerEngine::DuplicatedTitleUrlStorageType);
		EXPECT_EQ(0, pages.size());
	};

	env.initializeTest(testFunction);
	env.exec();
}

TEST(TitleTests, DuplicatedH1Title)
{
	// duplicated-h1-title.html -> duplicated-h1-title-2.html
	TestEnvironment env(TestEnvironment::defaultOptions(CustomUrl("http://title.com/duplicated-h1-title.html")));

	const auto testFunction = [cl = env.crawler()]()
	{
		auto pages = cl->waitForParsedPageReceived(CrawlerEngine::DuplicatedH1TitleUrlStorageType, 2, 10, "Waiting for 2 duplicated h1 title pages");
		EXPECT_EQ(2, pages.size());
		EXPECT_EQ(QString("Duplicated H1 Title"), pages[0]->title);
		EXPECT_EQ(QString("Duplicated H1 Title"), pages[1]->title);

		EXPECT_EQ(QString("Duplicated H1 Title"), pages[0]->firstH1);
		EXPECT_EQ(QString("Duplicated H1 Title"), pages[1]->firstH1);

		cl->waitForParsedPageReceived(CrawlerEngine::CrawledUrlStorageType, 2, 10, "Waiting for 2 crawled pages");
	};

	env.initializeTest(testFunction);
	env.exec();
}

TEST(TitleTests, SeveralTitles)
{
	// several-titles.html -> several-titles-2.html
	TestEnvironment env(TestEnvironment::defaultOptions(CustomUrl("http://title.com/several-titles.html")));

	const auto testFunction = [cl = env.crawler()]()
	{
		auto pages = cl->waitForParsedPageReceived(CrawlerEngine::SeveralTitleUrlStorageType, 2, 10, "Waiting for 2 several titles pages");
		EXPECT_EQ(2, pages.size());
		EXPECT_EQ(QString("First Title"), pages[0]->title);
		EXPECT_EQ(QString("First Title"), pages[1]->title);
		cl->waitForParsedPageReceived(CrawlerEngine::CrawledUrlStorageType, 2, 10, "Waiting for 2 crawled pages");
	};

	env.initializeTest(testFunction);
	env.exec();
}

}
