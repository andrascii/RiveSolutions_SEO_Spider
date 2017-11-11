namespace CrawlerEngineTests
{
	
TEST(H1AndH2Tests, EmptyH1)
{
	// empty-h1.html -> empty-h1-2.html

	TestEnvironment env(TestEnvironment::defaultOptions({ QUrl("http://h1h2.com/empty-h1.html") }));

	const auto testFunction = [cl = env.crawler()]()
	{
		auto pages = cl->waitForParsedPageReceived(CrawlerEngine::MissingH1UrlStorageType, 2, 10, "Waiting for 2 empty h1 pages");
		cl->checkSequencedDataCollectionConsistency();
		EXPECT_EQ(2, pages.size());
		EXPECT_EQ(QString(), pages[0]->firstH1);
		EXPECT_EQ(QString(), pages[1]->firstH1);
		cl->waitForParsedPageReceived(CrawlerEngine::CrawledUrlStorageType, 2, 10, "Waiting for 2 crawled pages");

		EXPECT_EQ(0, cl->storageItems(CrawlerEngine::DuplicatedH1UrlStorageType).size());
	};

	env.initializeTest(testFunction);
	env.exec();
}

TEST(H1AndH2Tests, NoH1)
{
	// no-h1.html -> no-h1-2.html

	TestEnvironment env(TestEnvironment::defaultOptions({ QUrl("http://h1h2.com/no-h1.html") }));

	const auto testFunction = [cl = env.crawler()]()
	{
		auto pages = cl->waitForParsedPageReceived(CrawlerEngine::MissingH1UrlStorageType, 2, 10, "Waiting for 2 empty h1 pages");
		cl->checkSequencedDataCollectionConsistency();
		EXPECT_EQ(2, pages.size());
		EXPECT_EQ(QString(), pages[0]->firstH1);
		EXPECT_EQ(QString(), pages[1]->firstH1);
		cl->waitForParsedPageReceived(CrawlerEngine::CrawledUrlStorageType, 2, 10, "Waiting for 2 crawled pages");

		EXPECT_EQ(0, cl->storageItems(CrawlerEngine::DuplicatedH1UrlStorageType).size());
	};

	env.initializeTest(testFunction);
	env.exec();
}

TEST(H1AndH2Tests, EmptyH2)
{
	// empty-h2.html -> empty-h2-2.html

	TestEnvironment env(TestEnvironment::defaultOptions({ QUrl("http://h1h2.com/empty-h2.html") }));

	const auto testFunction = [cl = env.crawler()]()
	{
		auto pages = cl->waitForParsedPageReceived(CrawlerEngine::MissingH2UrlStorageType, 2, 10, "Waiting for 2 empty h2 pages");
		cl->checkSequencedDataCollectionConsistency();
		EXPECT_EQ(2, pages.size());
		EXPECT_EQ(QString::null, pages[0]->firstH1);
		EXPECT_EQ(QString::null, pages[1]->firstH1);
		cl->waitForParsedPageReceived(CrawlerEngine::CrawledUrlStorageType, 2, 10, "Waiting for 2 crawled pages");

		EXPECT_EQ(0, cl->storageItems(CrawlerEngine::DuplicatedH2UrlStorageType).size());
	};

	env.initializeTest(testFunction);
	env.exec();
}

TEST(H1AndH2Tests, NoH2)
{
	// no-h2.html -> no-h2-2.html

	TestEnvironment env(TestEnvironment::defaultOptions({ QUrl("http://h1h2.com/no-h2.html") }));

	const auto testFunction = [cl = env.crawler()]()
	{
		auto pages = cl->waitForParsedPageReceived(CrawlerEngine::MissingH2UrlStorageType, 2, 10, "Waiting for 2 empty h2 pages");
		cl->checkSequencedDataCollectionConsistency();
		EXPECT_EQ(2, pages.size());
		EXPECT_EQ(QString::null, pages[0]->firstH1);
		EXPECT_EQ(QString::null, pages[1]->firstH1);
		cl->waitForParsedPageReceived(CrawlerEngine::CrawledUrlStorageType, 2, 10, "Waiting for 2 crawled pages");

		EXPECT_EQ(0, cl->storageItems(CrawlerEngine::DuplicatedH2UrlStorageType).size());
	};

	env.initializeTest(testFunction);
	env.exec();
}

TEST(H1AndH2Tests, DuplicateH1)
{
	// duplicate-h1.html -> duplicate-h1-2.html

	TestEnvironment env(TestEnvironment::defaultOptions({ QUrl("http://h1h2.com/duplicate-h1.html") }));

	const auto testFunction = [cl = env.crawler()]()
	{
		auto pages = cl->waitForParsedPageReceived(CrawlerEngine::DuplicatedH1UrlStorageType, 2, 10, "Waiting for 2 duplicate h1 pages");
		cl->checkSequencedDataCollectionConsistency();
		EXPECT_EQ(2, pages.size());
		EXPECT_EQ(QString("Duplicate H1"), pages[0]->firstH1);
		EXPECT_EQ(QString("Duplicate H1"), pages[1]->firstH1);
		cl->waitForParsedPageReceived(CrawlerEngine::CrawledUrlStorageType, 2, 10, "Waiting for 2 crawled pages");
	};

	env.initializeTest(testFunction);
	env.exec();
}

TEST(H1AndH2Tests, DuplicateH2)
{
	// duplicate-h2.html -> duplicate-h2-2.html

	TestEnvironment env(TestEnvironment::defaultOptions({ QUrl("http://h1h2.com/duplicate-h2.html") }));

	const auto testFunction = [cl = env.crawler()]()
	{
		auto pages = cl->waitForParsedPageReceived(CrawlerEngine::DuplicatedH2UrlStorageType, 2, 10, "Waiting for 2 duplicate h2 pages");
		cl->checkSequencedDataCollectionConsistency();
		EXPECT_EQ(2, pages.size());
		EXPECT_EQ(QString("Duplicate H2"), pages[0]->firstH2);
		EXPECT_EQ(QString("Duplicate H2"), pages[1]->firstH2);
		cl->waitForParsedPageReceived(CrawlerEngine::CrawledUrlStorageType, 2, 10, "Waiting for 2 crawled pages");
	};

	env.initializeTest(testFunction);
	env.exec();
}

TEST(H1AndH2Tests, TooLongH1)
{
	// too-long-h1.html -> too-long-h1-2.html

	CrawlerEngine::CrawlerOptions options = TestEnvironment::defaultOptions({ QUrl("http://h1h2.com/too-long-h1.html") });
	options.maxH1LengthChars = 10;

	TestEnvironment env(options);

	const auto testFunction = [cl = env.crawler()]()
	{
		auto pages = cl->waitForParsedPageReceived(CrawlerEngine::VeryLongH1UrlStorageType, 2, 10, "Waiting for 2 too long h1 pages");
		cl->checkSequencedDataCollectionConsistency();
		EXPECT_EQ(2, pages.size());
		EXPECT_EQ(QString("Too Long H1"), pages[0]->firstH1);
		EXPECT_EQ(QString("Another Long H1"), pages[1]->firstH1);
		cl->waitForParsedPageReceived(CrawlerEngine::CrawledUrlStorageType, 2, 10, "Waiting for 2 crawled pages");
	};

	env.initializeTest(testFunction);
	env.exec();
}

TEST(H1AndH2Tests, TooLongH2)
{
	// too-long-h2.html -> too-long-h2-2.html

	CrawlerEngine::CrawlerOptions options = TestEnvironment::defaultOptions({ QUrl("http://h1h2.com/too-long-h2.html") });
	options.maxH2LengthChars = 10;

	TestEnvironment env(options);

	const auto testFunction = [cl = env.crawler()]()
	{
		auto pages = cl->waitForParsedPageReceived(CrawlerEngine::VeryLongH2UrlStorageType, 2, 10, "Waiting for 2 too long h2 pages");
		cl->checkSequencedDataCollectionConsistency();
		EXPECT_EQ(2, pages.size());
		EXPECT_EQ(QString("Too Long H2"), pages[0]->firstH2);
		EXPECT_EQ(QString("Another Long H2"), pages[1]->firstH2);
		cl->waitForParsedPageReceived(CrawlerEngine::CrawledUrlStorageType, 2, 10, "Waiting for 2 crawled pages");
	};

	env.initializeTest(testFunction);
	env.exec();
}

TEST(H1AndH2Tests, SeveralEqualH1)
{
	// several-equal-h1.html -> several-equal-h1-2.html

	TestEnvironment env(TestEnvironment::defaultOptions({ QUrl("http://h1h2.com/several-equal-h1.html") }));

	const auto testFunction = [cl = env.crawler()]()
	{
		auto pages = cl->waitForParsedPageReceived(CrawlerEngine::SeveralH1UrlStorageType, 2, 10, "Waiting for 2 several equal h1 pages");
		cl->checkSequencedDataCollectionConsistency();
		EXPECT_EQ(2, pages.size());
		EXPECT_EQ(QString("First H1"), pages[0]->firstH1);
		EXPECT_EQ(QString("First H1-2"), pages[1]->firstH1);
		cl->waitForParsedPageReceived(CrawlerEngine::CrawledUrlStorageType, 2, 10, "Waiting for 2 crawled pages");
	};

	env.initializeTest(testFunction);
	env.exec();
}

TEST(H1AndH2Tests, SeveralEqual2)
{
	// several-equal-h2.html -> several-equal-h2-2.html

	TestEnvironment env(TestEnvironment::defaultOptions({ QUrl("http://h1h2.com/several-equal-h2.html") }));

	const auto testFunction = [cl = env.crawler()]()
	{
		auto pages = cl->waitForParsedPageReceived(CrawlerEngine::SeveralH2UrlStorageType, 2, 10, "Waiting for 2 several equal h2 pages");
		cl->checkSequencedDataCollectionConsistency();
		EXPECT_EQ(2, pages.size());
		EXPECT_EQ(QString("First H2"), pages[0]->firstH2);
		EXPECT_EQ(QString("First H2-2"), pages[1]->firstH2);
		cl->waitForParsedPageReceived(CrawlerEngine::CrawledUrlStorageType, 2, 10, "Waiting for 2 crawled pages");
	};

	env.initializeTest(testFunction);
	env.exec();
}

}