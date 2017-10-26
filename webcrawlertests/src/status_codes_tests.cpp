namespace WebCrawlerTests
{

TEST(StatusCodesTests, PagesWithNonSuccessfulCodesInSeoAnalysis)
{
	auto options = TestEnvironment::defaultOptions(QUrl("http://status.com/too-short.html"));
	options.maxTitleLength = 10;
	options.minTitleLength = 9;

	options.maxDescriptionLength = 10;
	options.minDescriptionLength = 9;

	options.maxH1LengthChars = 10;
	options.maxH2LengthChars = 10;

	TestEnvironment env(options);
	env.runTest([cl = env.crawler()]()
	{
		auto pages = cl->waitForParsedPageReceived(WebCrawler::CrawledUrlStorageType, 6, 10, "Waiting for 6 crawled pages");

		EXPECT_EQ(0, cl->storageItems(WebCrawler::EmptyTitleUrlStorageType).size());
		EXPECT_EQ(0, cl->storageItems(WebCrawler::DuplicatedTitleUrlStorageType).size());
		EXPECT_EQ(0, cl->storageItems(WebCrawler::VeryLongTitleUrlStorageType).size());
		EXPECT_EQ(0, cl->storageItems(WebCrawler::VeryShortTitleUrlStorageType).size());
		EXPECT_EQ(0, cl->storageItems(WebCrawler::DuplicatedH1TitleUrlStorageType).size());
		EXPECT_EQ(0, cl->storageItems(WebCrawler::SeveralTitleUrlStorageType).size());

		EXPECT_EQ(0, cl->storageItems(WebCrawler::EmptyMetaDescriptionUrlStorageType).size());
		EXPECT_EQ(0, cl->storageItems(WebCrawler::DuplicatedMetaDescriptionUrlStorageType).size());
		EXPECT_EQ(0, cl->storageItems(WebCrawler::VeryLongMetaDescriptionUrlStorageType).size());
		EXPECT_EQ(0, cl->storageItems(WebCrawler::VeryShortMetaDescriptionUrlStorageType).size());
		EXPECT_EQ(0, cl->storageItems(WebCrawler::SeveralMetaDescriptionUrlStorageType).size());

		EXPECT_EQ(0, cl->storageItems(WebCrawler::EmptyMetaKeywordsUrlStorageType).size());
		EXPECT_EQ(0, cl->storageItems(WebCrawler::DuplicatedMetaKeywordsUrlStorageType).size());
		EXPECT_EQ(0, cl->storageItems(WebCrawler::SeveralMetaKeywordsUrlStorageType).size());

		EXPECT_EQ(0, cl->storageItems(WebCrawler::MissingH1UrlStorageType).size());
		EXPECT_EQ(0, cl->storageItems(WebCrawler::DuplicatedH1UrlStorageType).size());
		EXPECT_EQ(0, cl->storageItems(WebCrawler::VeryLongH1UrlStorageType).size());
		EXPECT_EQ(0, cl->storageItems(WebCrawler::SeveralH1UrlStorageType).size());

		EXPECT_EQ(0, cl->storageItems(WebCrawler::MissingH2UrlStorageType).size());
		EXPECT_EQ(0, cl->storageItems(WebCrawler::DuplicatedH2UrlStorageType).size());
		EXPECT_EQ(0, cl->storageItems(WebCrawler::VeryLongH2UrlStorageType).size());
		EXPECT_EQ(0, cl->storageItems(WebCrawler::SeveralH2UrlStorageType).size());
	});
}

}