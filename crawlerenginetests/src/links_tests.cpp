namespace CrawlerEngineTests
{
	
TEST(LinksTests, LinkAlt)
{
	TestEnvironment env(TestEnvironment::defaultOptions(QUrl("http://links.com/link-with-title.html")));

	const auto testFunction = [cl = env.crawler()]()
	{
		auto pages = cl->waitForParsedPageReceived(CrawlerEngine::CrawledUrlStorageType, 2, 10, "Waiting for 2 crawled pages");
		cl->checkSequencedDataCollectionConsistency();
		EXPECT_EQ(2, pages.size());
		EXPECT_EQ(0, pages[0]->linksToThisPage.size());
		EXPECT_EQ(1, pages[0]->linksOnThisPage.size());
		EXPECT_EQ(Common::StatusCode::Ok200, pages[0]->statusCode);

		QString expectedTitle = QString::fromWCharArray(L"This is a broken link и чето по русски");
		EXPECT_EQ(expectedTitle, pages[0]->linksOnThisPage[0].altOrTitle);
	};

	env.initializeTest(testFunction);
	env.exec();
}

}