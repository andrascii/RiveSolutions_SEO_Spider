namespace CrawlerEngineTests
{

TEST(StatusCodesTests, PagesWithNonSuccessfulCodesInSeoAnalysis)
{
	int argc = 0;
	CrawlerEngine::CrawlerOptions options = TestEnvironment::defaultOptions(QUrl("http://status.com/too-short.html"));
	options.maxTitleLength = 10;
	options.minTitleLength = 9;

	options.maxDescriptionLength = 10;
	options.minDescriptionLength = 9;

	options.maxH1LengthChars = 10;
	options.maxH2LengthChars = 10;

	TestEnvironment env(argc, options);

	const auto testFunction = [cl = env.crawler()]()
	{
		auto pages = cl->waitForParsedPageReceived(CrawlerEngine::CrawledUrlStorageType, 6, 10, "Waiting for 6 crawled pages");

		EXPECT_EQ(0, cl->storageItems(CrawlerEngine::EmptyTitleUrlStorageType).size());
		EXPECT_EQ(0, cl->storageItems(CrawlerEngine::DuplicatedTitleUrlStorageType).size());
		EXPECT_EQ(0, cl->storageItems(CrawlerEngine::VeryLongTitleUrlStorageType).size());
		EXPECT_EQ(0, cl->storageItems(CrawlerEngine::VeryShortTitleUrlStorageType).size());
		EXPECT_EQ(0, cl->storageItems(CrawlerEngine::DuplicatedH1TitleUrlStorageType).size());
		EXPECT_EQ(0, cl->storageItems(CrawlerEngine::SeveralTitleUrlStorageType).size());

		EXPECT_EQ(0, cl->storageItems(CrawlerEngine::EmptyMetaDescriptionUrlStorageType).size());
		EXPECT_EQ(0, cl->storageItems(CrawlerEngine::DuplicatedMetaDescriptionUrlStorageType).size());
		EXPECT_EQ(0, cl->storageItems(CrawlerEngine::VeryLongMetaDescriptionUrlStorageType).size());
		EXPECT_EQ(0, cl->storageItems(CrawlerEngine::VeryShortMetaDescriptionUrlStorageType).size());
		EXPECT_EQ(0, cl->storageItems(CrawlerEngine::SeveralMetaDescriptionUrlStorageType).size());

		EXPECT_EQ(0, cl->storageItems(CrawlerEngine::EmptyMetaKeywordsUrlStorageType).size());
		EXPECT_EQ(0, cl->storageItems(CrawlerEngine::DuplicatedMetaKeywordsUrlStorageType).size());
		EXPECT_EQ(0, cl->storageItems(CrawlerEngine::SeveralMetaKeywordsUrlStorageType).size());

		EXPECT_EQ(0, cl->storageItems(CrawlerEngine::MissingH1UrlStorageType).size());
		EXPECT_EQ(0, cl->storageItems(CrawlerEngine::DuplicatedH1UrlStorageType).size());
		EXPECT_EQ(0, cl->storageItems(CrawlerEngine::VeryLongH1UrlStorageType).size());
		EXPECT_EQ(0, cl->storageItems(CrawlerEngine::SeveralH1UrlStorageType).size());

		EXPECT_EQ(0, cl->storageItems(CrawlerEngine::MissingH2UrlStorageType).size());
		EXPECT_EQ(0, cl->storageItems(CrawlerEngine::DuplicatedH2UrlStorageType).size());
		EXPECT_EQ(0, cl->storageItems(CrawlerEngine::VeryLongH2UrlStorageType).size());
		EXPECT_EQ(0, cl->storageItems(CrawlerEngine::SeveralH2UrlStorageType).size());
	};

	env.initializeTest(testFunction);
	env.exec();
}

}