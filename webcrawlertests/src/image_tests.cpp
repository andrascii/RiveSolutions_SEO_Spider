namespace WebCrawlerTests
{

TEST(ImageTests, EmptyAlt)
{
	// empty-alt.html(btclogo.png) -> empty-alt-2.html(btclogo-2.png)
	TestEnvironment env(TestEnvironment::defaultOptions(QUrl("http://image.com/empty-alt.html")));
	env.runTest([cl = env.crawler()]()
	{
		auto pages = cl->waitForParsedPageReceived(WebCrawler::MissingAltTextImageStorageType, 2, 10, "Waiting for 2 missing alt pages");
		EXPECT_EQ(2, pages.size());
		
		WebCrawler::ResourceLink linkToThisPage1 = TestEnvironment::firstResourceToThisPageOfType(pages[0], WebCrawler::ResourceType::ResourceHtml);
		WebCrawler::ResourceLink linkToThisPage2 = TestEnvironment::firstResourceToThisPageOfType(pages[1], WebCrawler::ResourceType::ResourceHtml);

		EXPECT_EQ(QString(""), linkToThisPage1.altOrTitle);
		EXPECT_EQ(WebCrawler::ResourceSource::SourceTagImg, linkToThisPage1.resourceSource);
		EXPECT_EQ(WebCrawler::LinkParameter::UnknownParameter, linkToThisPage1.urlParameter);
		EXPECT_EQ(0, pages[0]->linksOnThisPage.size());
		EXPECT_EQ(200, pages[0]->statusCode);

		EXPECT_EQ(QString(""), linkToThisPage2.altOrTitle);
		EXPECT_EQ(WebCrawler::ResourceSource::SourceTagImg, linkToThisPage2.resourceSource);
		EXPECT_EQ(WebCrawler::LinkParameter::UnknownParameter, linkToThisPage2.urlParameter);
		EXPECT_EQ(0, pages[1]->linksOnThisPage.size());
		EXPECT_EQ(200, pages[1]->statusCode);

		WebCrawler::ParsedPagePtr sourcePage1 = linkToThisPage1.resource.lock();
		WebCrawler::ParsedPagePtr sourcePage2 = linkToThisPage2.resource.lock();

		WebCrawler::ResourceLink linkOnThisPage1 = TestEnvironment::firstResourceOnThisPageOfType(sourcePage1, WebCrawler::ResourceType::ResourceImage);
		WebCrawler::ResourceLink linkOnThisPage2 = TestEnvironment::firstResourceOnThisPageOfType(sourcePage2, WebCrawler::ResourceType::ResourceImage);

		EXPECT_EQ(QString(""), linkOnThisPage1.altOrTitle);
		EXPECT_EQ(QString(""), linkOnThisPage2.altOrTitle);

		EXPECT_EQ(WebCrawler::ResourceSource::SourceTagImg, linkOnThisPage1.resourceSource);
		EXPECT_EQ(WebCrawler::ResourceSource::SourceTagImg, linkOnThisPage2.resourceSource);


		EXPECT_EQ(WebCrawler::LinkParameter::UnknownParameter, linkOnThisPage1.urlParameter);
		EXPECT_EQ(WebCrawler::LinkParameter::UnknownParameter, linkOnThisPage2.urlParameter);

		cl->waitForParsedPageReceived(WebCrawler::CrawledUrlStorageType, 4, 10, "Waiting for 4 crawled pages");
	});
}

TEST(ImageTests, NoAlt)
{
	// no-alt.html(btclogo.png) -> no-alt-2.html(btclogo-2.png)
	TestEnvironment env(TestEnvironment::defaultOptions(QUrl("http://image.com/no-alt.html")));
	env.runTest([cl = env.crawler()]()
	{
		auto pages = cl->waitForParsedPageReceived(WebCrawler::MissingAltTextImageStorageType, 2, 10, "Waiting for 2 missing alt pages");
		EXPECT_EQ(2, pages.size());

		WebCrawler::ResourceLink linkToThisPage1 = TestEnvironment::firstResourceToThisPageOfType(pages[0], WebCrawler::ResourceType::ResourceHtml);
		WebCrawler::ResourceLink linkToThisPage2 = TestEnvironment::firstResourceToThisPageOfType(pages[1], WebCrawler::ResourceType::ResourceHtml);

		EXPECT_EQ(QString(""), linkToThisPage1.altOrTitle);
		EXPECT_EQ(WebCrawler::ResourceSource::SourceTagImg, linkToThisPage1.resourceSource);
		EXPECT_EQ(WebCrawler::LinkParameter::UnknownParameter, linkToThisPage1.urlParameter);
		EXPECT_EQ(0, pages[0]->linksOnThisPage.size());
		EXPECT_EQ(200, pages[0]->statusCode);

		EXPECT_EQ(QString(""), linkToThisPage2.altOrTitle);
		EXPECT_EQ(WebCrawler::ResourceSource::SourceTagImg, linkToThisPage2.resourceSource);
		EXPECT_EQ(WebCrawler::LinkParameter::UnknownParameter, linkToThisPage2.urlParameter);
		EXPECT_EQ(0, pages[1]->linksOnThisPage.size());
		EXPECT_EQ(200, pages[1]->statusCode);

		WebCrawler::ParsedPagePtr sourcePage1 = linkToThisPage1.resource.lock();
		WebCrawler::ParsedPagePtr sourcePage2 = linkToThisPage2.resource.lock();

		WebCrawler::ResourceLink linkOnThisPage1 = TestEnvironment::firstResourceOnThisPageOfType(sourcePage1, WebCrawler::ResourceType::ResourceImage);
		WebCrawler::ResourceLink linkOnThisPage2 = TestEnvironment::firstResourceOnThisPageOfType(sourcePage2, WebCrawler::ResourceType::ResourceImage);

		EXPECT_EQ(QString(""), linkOnThisPage1.altOrTitle);
		EXPECT_EQ(QString(""), linkOnThisPage2.altOrTitle);

		EXPECT_EQ(WebCrawler::ResourceSource::SourceTagImg, linkOnThisPage1.resourceSource);
		EXPECT_EQ(WebCrawler::ResourceSource::SourceTagImg, linkOnThisPage2.resourceSource);


		EXPECT_EQ(WebCrawler::LinkParameter::UnknownParameter, linkOnThisPage1.urlParameter);
		EXPECT_EQ(WebCrawler::LinkParameter::UnknownParameter, linkOnThisPage2.urlParameter);

		cl->waitForParsedPageReceived(WebCrawler::CrawledUrlStorageType, 4, 10, "Waiting for 4 crawled pages");
	});
}

TEST(ImageTests, Image404)
{
	// image-404.html -> image-404-2.html
	TestEnvironment env(TestEnvironment::defaultOptions(QUrl("http://image.com/image-404.html")));
	env.runTest([cl = env.crawler()]()
	{
		auto pages = cl->waitForParsedPageReceived(WebCrawler::MissingAltTextImageStorageType, 2, 10, "Waiting for 2 missing alt pages");
		EXPECT_EQ(2, pages.size());

		WebCrawler::ResourceLink linkToThisPage1 = TestEnvironment::firstResourceToThisPageOfType(pages[0], WebCrawler::ResourceType::ResourceHtml);
		WebCrawler::ResourceLink linkToThisPage2 = TestEnvironment::firstResourceToThisPageOfType(pages[1], WebCrawler::ResourceType::ResourceHtml);

		EXPECT_EQ(QString(""), linkToThisPage1.altOrTitle);
		EXPECT_EQ(WebCrawler::ResourceSource::SourceTagImg, linkToThisPage1.resourceSource);
		EXPECT_EQ(WebCrawler::LinkParameter::UnknownParameter, linkToThisPage1.urlParameter);
		EXPECT_EQ(0, pages[0]->linksOnThisPage.size());
		EXPECT_EQ(404, pages[0]->statusCode);

		EXPECT_EQ(QString(""), linkToThisPage2.altOrTitle);
		EXPECT_EQ(WebCrawler::ResourceSource::SourceTagImg, linkToThisPage2.resourceSource);
		EXPECT_EQ(WebCrawler::LinkParameter::UnknownParameter, linkToThisPage2.urlParameter);
		EXPECT_EQ(0, pages[1]->linksOnThisPage.size());
		EXPECT_EQ(404, pages[1]->statusCode);

		WebCrawler::ParsedPagePtr sourcePage1 = linkToThisPage1.resource.lock();
		WebCrawler::ParsedPagePtr sourcePage2 = linkToThisPage2.resource.lock();

		WebCrawler::ResourceLink linkOnThisPage1 = TestEnvironment::firstResourceOnThisPageOfType(sourcePage1, WebCrawler::ResourceType::ResourceImage);
		WebCrawler::ResourceLink linkOnThisPage2 = TestEnvironment::firstResourceOnThisPageOfType(sourcePage2, WebCrawler::ResourceType::ResourceImage);

		EXPECT_EQ(QString(""), linkOnThisPage1.altOrTitle);
		EXPECT_EQ(QString(""), linkOnThisPage2.altOrTitle);

		EXPECT_EQ(WebCrawler::ResourceSource::SourceTagImg, linkOnThisPage1.resourceSource);
		EXPECT_EQ(WebCrawler::ResourceSource::SourceTagImg, linkOnThisPage2.resourceSource);


		auto pages404 = cl->waitForParsedPageReceived(WebCrawler::Status404StorageType, 2, 10, "Waiting for 2 404 status pages");
		EXPECT_EQ(2, pages404.size());

		auto pagesImage = cl->waitForParsedPageReceived(WebCrawler::ImageResourcesStorageType, 1, 10, "Waiting for 2 image pages");
		EXPECT_EQ(2, pagesImage.size());
	});
}

TEST(ImageTests, TwoPagesWithTheSameImage)
{
	TestEnvironment env(TestEnvironment::defaultOptions(QUrl("http://image.com/two-pages-with-same-image-1.html")));
	env.runTest([cl = env.crawler()]()
	{
		auto pages = cl->waitForParsedPageReceived(WebCrawler::CrawledUrlStorageType, 3, 10, "Waiting for 3 crawled pages");
		EXPECT_EQ(3, pages.size());

		auto images = cl->waitForParsedPageReceived(WebCrawler::ImageResourcesStorageType, 1, 10, "Waiting for 1 image page");

		auto changes = cl->waitForLinksToThisResourceChangesReceived(images[0], 2, 10);
		EXPECT_EQ(2, changes.size());
	});
}

TEST(ImageTests, ImageAlt)
{
	TestEnvironment env(TestEnvironment::defaultOptions(QUrl("http://image.com/image-alt.html")));
	env.runTest([cl = env.crawler()]()
	{
		auto pages = cl->waitForParsedPageReceived(WebCrawler::ImageResourcesStorageType, 1, 10, "Waiting for 1 image page");
		EXPECT_EQ(1, pages.size());
		EXPECT_EQ(1, pages[0]->linksToThisPage.size());
		EXPECT_EQ(0, pages[0]->missignAltIndices.size());
		EXPECT_EQ(0, pages[0]->linksOnThisPage.size());
		EXPECT_EQ(200, pages[0]->statusCode);

		QString expectedAlt = QString::fromWCharArray(L"Dummy Alt и чето по русски");
		EXPECT_EQ(expectedAlt, pages[0]->linksToThisPage[0].altOrTitle);
	});
}


TEST(ImageTests, TooBigImage)
{
	// too-big-image.html -> too-big-image-2.html
	auto options = TestEnvironment::defaultOptions(QUrl("http://image.com/too-big-image.html"));
	options.maxImageSizeKb = 0;
	TestEnvironment env(options);
	env.runTest([cl = env.crawler()]()
	{
		auto pages = cl->waitForParsedPageReceived(WebCrawler::Over100kbImageStorageType, 2, 100, "Waiting for 2 too big image page");

		EXPECT_EQ(2076, pages[0]->rawResponse.size());
		EXPECT_EQ(2076, pages[1]->rawResponse.size());

		cl->waitForParsedPageReceived(WebCrawler::CrawledUrlStorageType, 4, 10, "Waiting for 4 crawled pages");
	});
}


}
