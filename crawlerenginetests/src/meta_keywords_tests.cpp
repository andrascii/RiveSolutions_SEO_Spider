namespace CrawlerEngineTests
{

TEST(MetaKeywordsTests, EmptyMetaKeywords)
{
	// empty-meta.html -> empty-meta-2.html

	TestEnvironment env(TestEnvironment::defaultOptions({ QUrl("http://meta-keywords.com/empty-meta.html") }));

	const auto testFunction = [cl = env.crawler()]()
	{
		auto pages = cl->waitForParsedPageReceived(CrawlerEngine::EmptyMetaKeywordsUrlStorageType, 2, 10, "Waiting for 2 empty meta keywords pages");
		cl->checkSequencedDataCollectionConsistency();
		EXPECT_EQ(2, pages.size());
		EXPECT_EQ(QString(), pages[0]->metaKeywords);
		EXPECT_EQ(QString(), pages[1]->metaKeywords);
		cl->waitForParsedPageReceived(CrawlerEngine::CrawledUrlStorageType, 2, 10, "Waiting for 2 crawled pages");
	};

	env.initializeTest(testFunction);
	env.exec();
}


TEST(MetaKeywordsTests, NoMetaKeywords)
{
	// no-meta.html -> no-meta-2.html

	TestEnvironment env(TestEnvironment::defaultOptions({ QUrl("http://meta-keywords.com/no-meta.html") }));

	const auto testFunction = [cl = env.crawler()]()
	{
		auto pages = cl->waitForParsedPageReceived(CrawlerEngine::EmptyMetaKeywordsUrlStorageType, 2, 10, "Waiting for 2 empty meta keywords pages");
		cl->checkSequencedDataCollectionConsistency();
		EXPECT_EQ(2, pages.size());
		EXPECT_EQ(QString::null, pages[0]->metaKeywords);
		EXPECT_EQ(QString::null, pages[1]->metaKeywords);
		cl->waitForParsedPageReceived(CrawlerEngine::CrawledUrlStorageType, 2, 10, "Waiting for 2 crawled pages");
	};

	env.initializeTest(testFunction);
	env.exec();
}

TEST(MetaKeywordsTests, DuplicatedMetaKeywords)
{
	// duplicated-meta-1.html -> duplicated-meta-2.html

	CrawlerEngine::CrawlerOptions options = TestEnvironment::defaultOptions(QUrl("http://meta-keywords.com/duplicated-meta-1.html"));
	TestEnvironment env(options);

	const auto testFunction = [cl = env.crawler()]()
	{
		auto pages = cl->waitForParsedPageReceived(CrawlerEngine::DuplicatedMetaKeywordsUrlStorageType, 2, 10, "Waiting for 2 duplicated meta keywords pages");
		cl->checkSequencedDataCollectionConsistency();
		EXPECT_EQ(2, pages.size());
		EXPECT_EQ(QString("keyword1 keyword2"), pages[0]->metaKeywords);
		EXPECT_EQ(QString("keyword1 keyword2"), pages[1]->metaKeywords);
		cl->waitForParsedPageReceived(CrawlerEngine::CrawledUrlStorageType, 2, 10, "Waiting for 2 crawled pages");
	};

	env.initializeTest(testFunction);
	env.exec();
}

TEST(MetaKeywordsTests, DoNotIncludeCanonicalDuplicatedKeywords)
{
	// canonical-duplicated-key.html -> canonical-duplicated-key.html

	CrawlerEngine::CrawlerOptions options = TestEnvironment::defaultOptions(QUrl("http://meta-keywords.com/canonical-duplicated-key.html"));
	TestEnvironment env(options);

	const auto testFunction = [cl = env.crawler()]()
	{
		cl->waitForAllCrawledPageReceived(10);
		auto pages = cl->storageItems(CrawlerEngine::DuplicatedMetaKeywordsUrlStorageType);
		EXPECT_EQ(0, pages.size());
	};

	env.initializeTest(testFunction);
	env.exec();
}

TEST(MetaKeywordsTests, IncludeDuplicatedKeywordsIfThereAreSeveralCanonical)
{
	// canonical-duplicated-key-another.html -> canonical-desc-key.html -> canonical-duplicated-key.html

	CrawlerEngine::CrawlerOptions options = TestEnvironment::defaultOptions(QUrl("http://meta-keywords.com/canonical-duplicated-key-another.html"));
	TestEnvironment env(options);

	const auto testFunction = [cl = env.crawler()]()
	{
		cl->waitForAllCrawledPageReceived(10);
		auto pages = cl->storageItems(CrawlerEngine::DuplicatedMetaKeywordsUrlStorageType);
		EXPECT_EQ(3, pages.size());
	};

	env.initializeTest(testFunction);
	env.exec();
}

TEST(MetaKeywordsTests, DoNotIncludeEveryPageInDuplicates)
{
	CrawlerEngine::CrawlerOptions options = TestEnvironment::defaultOptions(QUrl("http://meta-keywords.com/single-page.html"));
	TestEnvironment env(options);

	const auto testFunction = [cl = env.crawler()]()
	{
		cl->waitForAllCrawledPageReceived(10);
		auto pages = cl->storageItems(CrawlerEngine::DuplicatedMetaKeywordsUrlStorageType);
		EXPECT_EQ(0, pages.size());
	};

	env.initializeTest(testFunction);
	env.exec();
}

TEST(MetaKeywordsTests, SeveralMetaKeywords)
{
	// several-titles.html -> several-titles-2.html

	TestEnvironment env(TestEnvironment::defaultOptions(QUrl("http://meta-keywords.com/several-meta.html")));

	const auto testFunction = [cl = env.crawler()]()
	{
		auto pages = cl->waitForParsedPageReceived(CrawlerEngine::SeveralMetaKeywordsUrlStorageType, 2, 10, "Waiting for 2 several meta keywords pages");
		cl->checkSequencedDataCollectionConsistency();
		EXPECT_EQ(2, pages.size());
		EXPECT_EQ(QString("keyword1 keyword2"), pages[0]->metaKeywords);
		EXPECT_EQ(QString("keyword1 keyword2"), pages[1]->metaKeywords);
		cl->waitForParsedPageReceived(CrawlerEngine::CrawledUrlStorageType, 2, 10, "Waiting for 2 crawled pages");
	};

	env.initializeTest(testFunction);
	env.exec();
}

}