namespace WebCrawlerTests
{

TEST(ImageTests, EmptyAlt)
{
	WebCrawlerTests::TestEnvironment env(TestEnvironment::defaultOptions(QUrl("http://image.com/empty-alt.html")));
	env.runTest([cl = env.crawler()]()
	{
		auto pages = cl->waitForParsedPageReceived(WebCrawler::MissingAltTextImageStorageType, 1, 10);
		EXPECT_EQ(1, pages.size());
		EXPECT_EQ(1, pages[0]->linksToThisPage.size());
		EXPECT_EQ(QString(""), pages[0]->linksToThisPage[0].altOrTitle);

		WebCrawler::ParsedPagePtr sourcePage = pages[0]->linksToThisPage[0].resource.lock();
		EXPECT_EQ(1, sourcePage->linksOnThisPage.size());
		EXPECT_EQ(QString(""), sourcePage->linksOnThisPage[0].altOrTitle);
	});
}

TEST(ImageTests, NoAlt)
{
	WebCrawlerTests::TestEnvironment env(TestEnvironment::defaultOptions(QUrl("http://image.com/no-alt.html")));
	env.runTest([cl = env.crawler()]()
	{
		auto pages = cl->waitForParsedPageReceived(WebCrawler::MissingAltTextImageStorageType, 1, 10);
		EXPECT_EQ(1, pages.size());
		EXPECT_EQ(1, pages[0]->linksToThisPage.size());
		EXPECT_EQ(QString(""), pages[0]->linksToThisPage[0].altOrTitle);

		WebCrawler::ParsedPagePtr sourcePage = pages[0]->linksToThisPage[0].resource.lock();
		EXPECT_EQ(1, sourcePage->linksOnThisPage.size());
		EXPECT_EQ(QString(""), sourcePage->linksOnThisPage[0].altOrTitle);
	});
}

}
