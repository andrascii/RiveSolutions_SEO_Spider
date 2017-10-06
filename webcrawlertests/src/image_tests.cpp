namespace WebCrawlerTests
{

TEST(ImageTests, EmptyAlt)
{
	TestEnvironment env(TestEnvironment::defaultOptions(QUrl("http://image.com/empty-alt.html")));
	env.runTest([cl = env.crawler()]()
	{
		auto pages = cl->waitForParsedPageReceived(WebCrawler::MissingAltTextImageStorageType, 1, 10);
		EXPECT_EQ(1, pages.size());
		EXPECT_EQ(1, pages[0]->linksToThisPage.size());
		EXPECT_EQ(QString(""), pages[0]->linksToThisPage[0].altOrTitle);
		EXPECT_EQ(0, pages[0]->linksOnThisPage.size());
		EXPECT_EQ(200, pages[0]->statusCode);

		WebCrawler::ParsedPagePtr sourcePage = pages[0]->linksToThisPage[0].resource.lock();
		EXPECT_EQ(1, sourcePage->linksOnThisPage.size());
		EXPECT_EQ(QString(""), sourcePage->linksOnThisPage[0].altOrTitle);
	});
}

TEST(ImageTests, NoAlt)
{
	TestEnvironment env(TestEnvironment::defaultOptions(QUrl("http://image.com/no-alt.html")));
	env.runTest([cl = env.crawler()]()
	{
		auto pages = cl->waitForParsedPageReceived(WebCrawler::MissingAltTextImageStorageType, 1, 10);
		EXPECT_EQ(1, pages.size());
		EXPECT_EQ(1, pages[0]->linksToThisPage.size());
		EXPECT_EQ(1, pages[0]->missignAltIndices.size());
		EXPECT_EQ(0, pages[0]->missignAltIndices[0]);
		EXPECT_EQ(QString(""), pages[0]->linksToThisPage[0].altOrTitle);
		EXPECT_EQ(0, pages[0]->linksOnThisPage.size());
		EXPECT_EQ(200, pages[0]->statusCode);

		WebCrawler::ParsedPagePtr sourcePage = pages[0]->linksToThisPage[0].resource.lock();
		EXPECT_EQ(1, sourcePage->linksOnThisPage.size());
		EXPECT_EQ(QString(""), sourcePage->linksOnThisPage[0].altOrTitle);
	});
}

TEST(ImageTests, Image404)
{
	TestEnvironment env(TestEnvironment::defaultOptions(QUrl("http://image.com/image-404.html")));
	env.runTest([cl = env.crawler()]()
	{
		auto pages = cl->waitForParsedPageReceived(WebCrawler::MissingAltTextImageStorageType, 1, 10);
		EXPECT_EQ(1, pages.size());
		EXPECT_EQ(1, pages[0]->linksToThisPage.size());
		EXPECT_EQ(1, pages[0]->missignAltIndices.size());
		EXPECT_EQ(0, pages[0]->missignAltIndices[0]);
		EXPECT_EQ(0, pages[0]->linksOnThisPage.size());
		EXPECT_EQ(404, pages[0]->statusCode);

		auto pages404 = cl->waitForParsedPageReceived(WebCrawler::Status404StorageType, 1, 10);
		EXPECT_EQ(1, pages404.size());

		auto pagesImage = cl->waitForParsedPageReceived(WebCrawler::ImageResourcesStorageType, 1, 10);
		EXPECT_EQ(1, pagesImage.size());
	});
}

TEST(ImageTests, TwoPagesWithTheSameImage)
{
	TestEnvironment env(TestEnvironment::defaultOptions(QUrl("http://image.com/two-pages-with-same-image-1.html")));
	env.runTest([cl = env.crawler()]()
	{
		auto pages = cl->waitForParsedPageReceived(WebCrawler::CrawledUrlStorageType, 3, 10);
		EXPECT_EQ(3, pages.size());

		auto images = cl->waitForParsedPageReceived(WebCrawler::ImageResourcesStorageType, 1, 10);

		auto changes = cl->waitForLinksToThisResourceChangesReceived(images[0], 2, 10);
		EXPECT_EQ(2, changes.size());
	});
}

TEST(ImageTests, ImageAlt)
{
	TestEnvironment env(TestEnvironment::defaultOptions(QUrl("http://image.com/image-alt.html")));
	env.runTest([cl = env.crawler()]()
	{
		auto pages = cl->waitForParsedPageReceived(WebCrawler::ImageResourcesStorageType, 1, 10);
		EXPECT_EQ(1, pages.size());
		EXPECT_EQ(1, pages[0]->linksToThisPage.size());
		EXPECT_EQ(0, pages[0]->missignAltIndices.size());
		EXPECT_EQ(0, pages[0]->linksOnThisPage.size());
		EXPECT_EQ(200, pages[0]->statusCode);

		QString expectedAlt = QString::fromWCharArray(L"Dummy Alt и чето по русски");
		EXPECT_EQ(expectedAlt, pages[0]->linksToThisPage[0].altOrTitle);
	});
}

}
