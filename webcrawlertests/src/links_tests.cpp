namespace WebCrawlerTests
{
	
TEST(LinksTests, LinkAlt)
{
	TestEnvironment env(TestEnvironment::defaultOptions(QUrl("http://links.com/link-with-title.html")));
	env.runTest([cl = env.crawler()]()
	{
		auto pages = cl->waitForParsedPageReceived(WebCrawler::CrawledUrlStorageType, 2, 10, "Waiting for 2 crawled pages");
		EXPECT_EQ(2, pages.size());
		EXPECT_EQ(0, pages[0]->linksToThisPage.size());
		EXPECT_EQ(1, pages[0]->linksOnThisPage.size());
		EXPECT_EQ(Common::ResponseCode::Ok200, pages[0]->responseCode);

		QString expectedTitle = QString::fromWCharArray(L"This is a broken link и чето по русски");
		EXPECT_EQ(expectedTitle, pages[0]->linksOnThisPage[0].altOrTitle);
	});
}

}