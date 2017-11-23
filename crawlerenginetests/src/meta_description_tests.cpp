namespace CrawlerEngineTests
{

TEST(MetaDescriptionTests, EmptyMetaDescription)
{
	// empty-meta.html -> empty-meta-2.html

	std::lock_guard<std::mutex> locker(g_mutex);

	TestEnvironment env(TestEnvironment::defaultOptions({ QUrl("http://meta-desc.com/empty-meta.html") }));

	const auto testFunction = [cl = env.crawler()]()
	{
		auto pages = cl->waitForParsedPageReceived(CrawlerEngine::EmptyMetaDescriptionUrlStorageType, 2, 10, "Waiting for 2 empty meta description pages");
		cl->checkSequencedDataCollectionConsistency();
		EXPECT_EQ(2, pages.size());
		EXPECT_EQ(QString(), pages[0]->metaDescription);
		EXPECT_EQ(QString(), pages[1]->metaDescription);
		cl->waitForParsedPageReceived(CrawlerEngine::CrawledUrlStorageType, 2, 10, "Waiting for 2 crawled pages");
	};

	env.initializeTest(testFunction);
	env.exec();
}


TEST(MetaDescriptionTests, NoMetaDescription)
{
	// no-meta.html -> no-meta-2.html

	std::lock_guard<std::mutex> locker(g_mutex);

	TestEnvironment env(TestEnvironment::defaultOptions({ QUrl("http://meta-desc.com/no-meta.html") }));

	const auto testFunction = [cl = env.crawler()]()
	{
		auto pages = cl->waitForParsedPageReceived(CrawlerEngine::EmptyMetaDescriptionUrlStorageType, 2, 10, "Waiting for 2 empty meta description pages");
		cl->checkSequencedDataCollectionConsistency();
		EXPECT_EQ(2, pages.size());
		EXPECT_EQ(QString::null, pages[0]->metaDescription);
		EXPECT_EQ(QString::null, pages[1]->metaDescription);
		cl->waitForParsedPageReceived(CrawlerEngine::CrawledUrlStorageType, 2, 10, "Waiting for 2 crawled pages");
	};

	env.initializeTest(testFunction);
	env.exec();
}


TEST(MetaDescriptionTests, TooLongMetaDescription)
{
	// too-long-meta.html -> too-long-meta-2.html

	std::lock_guard<std::mutex> locker(g_mutex);

	CrawlerEngine::CrawlerOptions options = TestEnvironment::defaultOptions(QUrl("http://meta-desc.com/too-long-meta.html"));
	options.maxDescriptionLength = 10;
	TestEnvironment env( options);

	const auto testFunction = [cl = env.crawler()]()
	{
		auto pages = cl->waitForParsedPageReceived(CrawlerEngine::VeryLongMetaDescriptionUrlStorageType, 2, 10, "Waiting for 2 too long meta desc pages");
		cl->checkSequencedDataCollectionConsistency();
		EXPECT_EQ(2, pages.size());
		EXPECT_EQ(QString("This is too long meta description"), pages[0]->metaDescription);
		EXPECT_EQ(QString("This is too long meta description"), pages[1]->metaDescription);
		cl->waitForParsedPageReceived(CrawlerEngine::CrawledUrlStorageType, 2, 10, "Waiting for 2 crawled pages");
	};

	env.initializeTest(testFunction);
	env.exec();
}

TEST(MetaDescriptionTests, TooShortMetaDescription)
{
	// too-short-meta.html -> too-short-meta-2.html

	std::lock_guard<std::mutex> locker(g_mutex);

	CrawlerEngine::CrawlerOptions options = TestEnvironment::defaultOptions(QUrl("http://meta-desc.com/too-short-meta.html"));
	options.minDescriptionLength = 50;
	options.maxDescriptionLength = 100;
	TestEnvironment env(options);

	const auto testFunction = [cl = env.crawler()]()
	{
		auto pages = cl->waitForParsedPageReceived(CrawlerEngine::VeryShortMetaDescriptionUrlStorageType, 2, 10, "Waiting for 2 too short meta desc pages");
		cl->checkSequencedDataCollectionConsistency();
		EXPECT_EQ(2, pages.size());
		EXPECT_EQ(QString("This is too short meta description"), pages[0]->metaDescription);
		EXPECT_EQ(QString("This is too short meta description"), pages[1]->metaDescription);
		cl->waitForParsedPageReceived(CrawlerEngine::CrawledUrlStorageType, 2, 10, "Waiting for 2 crawled pages");
	};

	env.initializeTest(testFunction);
	env.exec();
}

TEST(MetaDescriptionTests, DuplicatedMetaDescriptions)
{
	// duplicated-meta-1.html -> duplicated-meta-2.html

	std::lock_guard<std::mutex> locker(g_mutex);

	CrawlerEngine::CrawlerOptions options = TestEnvironment::defaultOptions(QUrl("http://meta-desc.com/duplicated-meta-1.html"));
	TestEnvironment env(options);

	const auto testFunction = [cl = env.crawler()]()
	{
		auto pages = cl->waitForParsedPageReceived(CrawlerEngine::DuplicatedMetaDescriptionUrlStorageType, 2, 10, "Waiting for 2 duplicated meta desc pages");
		cl->checkSequencedDataCollectionConsistency();
		EXPECT_EQ(2, pages.size());
		EXPECT_EQ(QString("Duplicated Meta Description"), pages[0]->metaDescription);
		EXPECT_EQ(QString("Duplicated Meta Description"), pages[1]->metaDescription);
		cl->waitForParsedPageReceived(CrawlerEngine::CrawledUrlStorageType, 2, 10, "Waiting for 2 crawled pages");
	};

	env.initializeTest(testFunction);
	env.exec();
}

TEST(MetaDescriptionTests, DoNotIncludeCanonicalDuplicatedTitles)
{
	// canonical-duplicated-desc.html -> canonical-duplicated-desc.html

	CrawlerEngine::CrawlerOptions options = TestEnvironment::defaultOptions(QUrl("http://meta-desc.com/canonical-duplicated-desc.html"));
	TestEnvironment env(options);

	const auto testFunction = [cl = env.crawler()]()
	{
		cl->waitForAllCrawledPageReceived(10);
		auto pages = cl->storageItems(CrawlerEngine::DuplicatedMetaDescriptionUrlStorageType);
		EXPECT_EQ(0, pages.size());
	};

	env.initializeTest(testFunction);
	env.exec();
}

TEST(MetaDescriptionTests, IncludeDuplicatedTitlesIfThereAreSeveralCanonical)
{
	// canonical-duplicated-desc-another.html -> canonical-desc-title.html -> canonical-duplicated-desc.html

	CrawlerEngine::CrawlerOptions options = TestEnvironment::defaultOptions(QUrl("http://meta-desc.com/canonical-duplicated-desc-another.html"));
	TestEnvironment env(options);

	const auto testFunction = [cl = env.crawler()]()
	{
		cl->waitForAllCrawledPageReceived(10);
		auto pages = cl->storageItems(CrawlerEngine::DuplicatedMetaDescriptionUrlStorageType);
		EXPECT_EQ(3, pages.size());
	};

	env.initializeTest(testFunction);
	env.exec();
}

TEST(MetaDescriptionTests, DoNotIncludeEveryPageInDuplicates)
{
	CrawlerEngine::CrawlerOptions options = TestEnvironment::defaultOptions(QUrl("http://meta-desc.com/single-page.html"));
	TestEnvironment env(options);

	const auto testFunction = [cl = env.crawler()]()
	{
		cl->waitForAllCrawledPageReceived(10);
		auto pages = cl->storageItems(CrawlerEngine::DuplicatedMetaDescriptionUrlStorageType);
		EXPECT_EQ(0, pages.size());
	};

	env.initializeTest(testFunction);
	env.exec();
}

TEST(MetaDescriptionTests, SeveralMetaDescriptions)
{
	// several-titles.html -> several-titles-2.html

	std::lock_guard<std::mutex> locker(g_mutex);

	TestEnvironment env(TestEnvironment::defaultOptions(QUrl("http://meta-desc.com/several-meta.html")));

	const auto testFunction = [cl = env.crawler()]()
	{
		auto pages = cl->waitForParsedPageReceived(CrawlerEngine::SeveralMetaDescriptionUrlStorageType, 2, 10, "Waiting for 2 several meta desc pages");
		cl->checkSequencedDataCollectionConsistency();
		EXPECT_EQ(2, pages.size());
		EXPECT_EQ(QString("First Meta Description"), pages[0]->metaDescription);
		EXPECT_EQ(QString("First Meta Description"), pages[1]->metaDescription);
		cl->waitForParsedPageReceived(CrawlerEngine::CrawledUrlStorageType, 2, 10, "Waiting for 2 crawled pages");
	};

	env.initializeTest(testFunction);
	env.exec();
}

}