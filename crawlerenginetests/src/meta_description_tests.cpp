namespace CrawlerEngineTests
{

TEST(MetaDescriptionTests, EmptyMetaDescription)
{
	// empty-meta.html -> empty-meta-2.html
	TestEnvironment env(TestEnvironment::defaultOptions({ QUrl("http://meta-desc.com/empty-meta.html") }));
	env.runTest([cl = env.crawler()]()
	{
		auto pages = cl->waitForParsedPageReceived(CrawlerEngine::EmptyMetaDescriptionUrlStorageType, 2, 10, "Waiting for 2 empty meta description pages");
		EXPECT_EQ(2, pages.size());
		EXPECT_EQ(QString(""), pages[0]->metaDescription);
		EXPECT_EQ(QString(""), pages[1]->metaDescription);
		cl->waitForParsedPageReceived(CrawlerEngine::CrawledUrlStorageType, 2, 10, "Waiting for 2 crawled pages");
	});
}


TEST(MetaDescriptionTests, NoMetaDescription)
{
	// no-meta.html -> no-meta-2.html
	TestEnvironment env(TestEnvironment::defaultOptions({ QUrl("http://meta-desc.com/no-meta.html") }));
	env.runTest([cl = env.crawler()]()
	{
		auto pages = cl->waitForParsedPageReceived(CrawlerEngine::EmptyMetaDescriptionUrlStorageType, 2, 10, "Waiting for 2 empty meta description pages");
		EXPECT_EQ(2, pages.size());
		EXPECT_EQ(QString(""), pages[0]->metaDescription);
		EXPECT_EQ(QString(""), pages[1]->metaDescription);
		cl->waitForParsedPageReceived(CrawlerEngine::CrawledUrlStorageType, 2, 10, "Waiting for 2 crawled pages");
	});
}


TEST(MetaDescriptionTests, TooLongMetaDescription)
{
	// too-long-meta.html -> too-long-meta-2.html
	CrawlerEngine::CrawlerOptions options = TestEnvironment::defaultOptions(QUrl("http://meta-desc.com/too-long-meta.html"));
	options.maxDescriptionLength = 10;
	TestEnvironment env(options);
	env.runTest([cl = env.crawler()]()
	{
		auto pages = cl->waitForParsedPageReceived(CrawlerEngine::VeryLongMetaDescriptionUrlStorageType, 2, 10, "Waiting for 2 too long meta desc pages");
		EXPECT_EQ(2, pages.size());
		EXPECT_EQ(QString("This is too long meta description"), pages[0]->metaDescription);
		EXPECT_EQ(QString("This is too long meta description"), pages[1]->metaDescription);
		cl->waitForParsedPageReceived(CrawlerEngine::CrawledUrlStorageType, 2, 10, "Waiting for 2 crawled pages");
	});
}

TEST(MetaDescriptionTests, TooShortMetaDescription)
{
	// too-short-meta.html -> too-short-meta-2.html
	CrawlerEngine::CrawlerOptions options = TestEnvironment::defaultOptions(QUrl("http://meta-desc.com/too-short-meta.html"));
	options.minDescriptionLength = 50;
	options.maxDescriptionLength = 100;
	TestEnvironment env(options);
	env.runTest([cl = env.crawler()]()
	{
		auto pages = cl->waitForParsedPageReceived(CrawlerEngine::VeryShortMetaDescriptionUrlStorageType, 2, 10, "Waiting for 2 too short meta desc pages");
		EXPECT_EQ(2, pages.size());
		EXPECT_EQ(QString("This is too short meta description"), pages[0]->metaDescription);
		EXPECT_EQ(QString("This is too short meta description"), pages[1]->metaDescription);
		cl->waitForParsedPageReceived(CrawlerEngine::CrawledUrlStorageType, 2, 10, "Waiting for 2 crawled pages");
	});
}

TEST(MetaDescriptionTests, DuplicatedMetaDescriptions)
{
	// duplicated-meta-1.html -> duplicated-meta-2.html
	CrawlerEngine::CrawlerOptions options = TestEnvironment::defaultOptions(QUrl("http://meta-desc.com/duplicated-meta-1.html"));
	TestEnvironment env(options);
	env.runTest([cl = env.crawler()]()
	{
		auto pages = cl->waitForParsedPageReceived(CrawlerEngine::DuplicatedMetaDescriptionUrlStorageType, 2, 10, "Waiting for 2 duplicated meta desc pages");
		EXPECT_EQ(2, pages.size());
		EXPECT_EQ(QString("Duplicated Meta Description"), pages[0]->metaDescription);
		EXPECT_EQ(QString("Duplicated Meta Description"), pages[1]->metaDescription);
		cl->waitForParsedPageReceived(CrawlerEngine::CrawledUrlStorageType, 2, 10, "Waiting for 2 crawled pages");
	});
}

TEST(MetaDescriptionTests, SeveralMetaDescriptions)
{
	// several-titles.html -> several-titles-2.html
	TestEnvironment env(TestEnvironment::defaultOptions(QUrl("http://meta-desc.com/several-meta.html")));
	env.runTest([cl = env.crawler()]()
	{
		auto pages = cl->waitForParsedPageReceived(CrawlerEngine::SeveralMetaDescriptionUrlStorageType, 2, 10, "Waiting for 2 several meta desc pages");
		EXPECT_EQ(2, pages.size());
		EXPECT_EQ(QString("First Meta Description"), pages[0]->metaDescription);
		EXPECT_EQ(QString("First Meta Description"), pages[1]->metaDescription);
		cl->waitForParsedPageReceived(CrawlerEngine::CrawledUrlStorageType, 2, 10, "Waiting for 2 crawled pages");
	});
}

}