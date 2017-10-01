namespace WebCrawlerTests
{

TEST(ImageTests, EmptyAlt)
{
	WebCrawlerTests::TestEnvironment env(TestEnvironment::defaultOptions(QUrl("http://image.com/empty-alt.html")));
	env.runTest([cl = env.crawler()]()
	{
		auto pages = cl->waitForParsedPageReceived(WebCrawler::MissingAltTextImageStorageType, 1, 10);
		EXPECT_EQ(1, pages.size());
		EXPECT_EQ(QString(""), pages[0]->altText);
	});
}

}
