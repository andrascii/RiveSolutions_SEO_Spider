namespace CrawlerEngineTests
{

TEST(ImageTests, EmptyAlt)
{
	// empty-alt.html(btclogo.png) -> empty-alt-2.html(btclogo-2.png)

	TestEnvironment env(TestEnvironment::defaultOptions(QUrl("http://image.com/empty-alt.html")));

	const auto testFunction = [cl = env.crawler()]()
	{
		auto pages = cl->waitForParsedPageReceived(CrawlerEngine::MissingAltTextImageStorageType, 2, 10, "Waiting for 2 missing alt pages");
		cl->checkSequencedDataCollectionConsistency();
		EXPECT_EQ(2, pages.size());

		CrawlerEngine::ResourceLink linkToThisPage1 = TestEnvironment::firstResourceToThisPageOfType(pages[0], CrawlerEngine::ResourceType::ResourceHtml);
		CrawlerEngine::ResourceLink linkToThisPage2 = TestEnvironment::firstResourceToThisPageOfType(pages[1], CrawlerEngine::ResourceType::ResourceHtml);

		EXPECT_EQ(QString(), linkToThisPage1.altOrTitle);
		EXPECT_EQ(CrawlerEngine::ResourceSource::SourceTagImg, linkToThisPage1.resourceSource);
		EXPECT_EQ(CrawlerEngine::LinkParameter::DofollowParameter, linkToThisPage1.linkParameter);
		EXPECT_EQ(0, pages[0]->linksOnThisPage.size());
		EXPECT_EQ(Common::StatusCode::Ok200, pages[0]->statusCode);

		EXPECT_EQ(QString(), linkToThisPage2.altOrTitle);
		EXPECT_EQ(CrawlerEngine::ResourceSource::SourceTagImg, linkToThisPage2.resourceSource);
		EXPECT_EQ(CrawlerEngine::LinkParameter::DofollowParameter, linkToThisPage2.linkParameter);
		EXPECT_EQ(0, pages[1]->linksOnThisPage.size());
		EXPECT_EQ(Common::StatusCode::Ok200, pages[1]->statusCode);

		const CrawlerEngine::ParsedPage* sourcePage1 = linkToThisPage1.resource.lock().get();
		const CrawlerEngine::ParsedPage* sourcePage2 = linkToThisPage2.resource.lock().get();

		CrawlerEngine::ResourceLink linkOnThisPage1 = TestEnvironment::firstResourceOnThisPageOfType(sourcePage1, CrawlerEngine::ResourceType::ResourceImage);
		CrawlerEngine::ResourceLink linkOnThisPage2 = TestEnvironment::firstResourceOnThisPageOfType(sourcePage2, CrawlerEngine::ResourceType::ResourceImage);

		EXPECT_EQ(QString(), linkOnThisPage1.altOrTitle);
		EXPECT_EQ(QString(), linkOnThisPage2.altOrTitle);

		EXPECT_EQ(CrawlerEngine::ResourceSource::SourceTagImg, linkOnThisPage1.resourceSource);
		EXPECT_EQ(CrawlerEngine::ResourceSource::SourceTagImg, linkOnThisPage2.resourceSource);


		EXPECT_EQ(CrawlerEngine::LinkParameter::DofollowParameter, linkOnThisPage1.linkParameter);
		EXPECT_EQ(CrawlerEngine::LinkParameter::DofollowParameter, linkOnThisPage2.linkParameter);

		cl->waitForParsedPageReceived(CrawlerEngine::CrawledUrlStorageType, 4, 10, "Waiting for 4 crawled pages");
	};

	env.initializeTest(testFunction);
	env.exec();
}

TEST(ImageTests, NoAlt)
{
	// no-alt.html(btclogo.png) -> no-alt-2.html(btclogo-2.png)
	int argc = 0;
	TestEnvironment env(TestEnvironment::defaultOptions(QUrl("http://image.com/no-alt.html")));

	const auto testFunction = [cl = env.crawler()]()
	{
		auto pages = cl->waitForParsedPageReceived(CrawlerEngine::MissingAltTextImageStorageType, 2, 10, "Waiting for 2 missing alt pages");
		cl->checkSequencedDataCollectionConsistency();
		EXPECT_EQ(2, pages.size());

		CrawlerEngine::ResourceLink linkToThisPage1 = TestEnvironment::firstResourceToThisPageOfType(pages[0], CrawlerEngine::ResourceType::ResourceHtml);
		CrawlerEngine::ResourceLink linkToThisPage2 = TestEnvironment::firstResourceToThisPageOfType(pages[1], CrawlerEngine::ResourceType::ResourceHtml);

		EXPECT_EQ(QString(), linkToThisPage1.altOrTitle);
		EXPECT_EQ(CrawlerEngine::ResourceSource::SourceTagImg, linkToThisPage1.resourceSource);
		EXPECT_EQ(CrawlerEngine::LinkParameter::DofollowParameter, linkToThisPage1.linkParameter);
		EXPECT_EQ(0, pages[0]->linksOnThisPage.size());
		EXPECT_EQ(Common::StatusCode::Ok200, pages[0]->statusCode);

		EXPECT_EQ(QString(), linkToThisPage2.altOrTitle);
		EXPECT_EQ(CrawlerEngine::ResourceSource::SourceTagImg, linkToThisPage2.resourceSource);
		EXPECT_EQ(CrawlerEngine::LinkParameter::DofollowParameter, linkToThisPage2.linkParameter);
		EXPECT_EQ(0, pages[1]->linksOnThisPage.size());
		EXPECT_EQ(Common::StatusCode::Ok200, pages[1]->statusCode);

		const CrawlerEngine::ParsedPage* sourcePage1 = linkToThisPage1.resource.lock().get();
		const CrawlerEngine::ParsedPage* sourcePage2 = linkToThisPage2.resource.lock().get();

		CrawlerEngine::ResourceLink linkOnThisPage1 = TestEnvironment::firstResourceOnThisPageOfType(sourcePage1, CrawlerEngine::ResourceType::ResourceImage);
		CrawlerEngine::ResourceLink linkOnThisPage2 = TestEnvironment::firstResourceOnThisPageOfType(sourcePage2, CrawlerEngine::ResourceType::ResourceImage);

		EXPECT_EQ(QString(), linkOnThisPage1.altOrTitle);
		EXPECT_EQ(QString(), linkOnThisPage2.altOrTitle);

		EXPECT_EQ(CrawlerEngine::ResourceSource::SourceTagImg, linkOnThisPage1.resourceSource);
		EXPECT_EQ(CrawlerEngine::ResourceSource::SourceTagImg, linkOnThisPage2.resourceSource);


		EXPECT_EQ(CrawlerEngine::LinkParameter::DofollowParameter, linkOnThisPage1.linkParameter);
		EXPECT_EQ(CrawlerEngine::LinkParameter::DofollowParameter, linkOnThisPage2.linkParameter);

		cl->waitForParsedPageReceived(CrawlerEngine::CrawledUrlStorageType, 4, 10, "Waiting for 4 crawled pages");
	};

	env.initializeTest(testFunction);
	env.exec();
}

TEST(ImageTests, Image404)
{
	// image-404.html -> image-404-2.html
	TestEnvironment env(TestEnvironment::defaultOptions(QUrl("http://image.com/image-404.html")));

	const auto testFunction = [cl = env.crawler()]()
	{
		auto pages = cl->waitForParsedPageReceived(CrawlerEngine::MissingAltTextImageStorageType, 2, 10, "Waiting for 2 missing alt pages");
		cl->checkSequencedDataCollectionConsistency();
		EXPECT_EQ(2, pages.size());

		CrawlerEngine::ResourceLink linkToThisPage1 = TestEnvironment::firstResourceToThisPageOfType(pages[0], CrawlerEngine::ResourceType::ResourceHtml);
		CrawlerEngine::ResourceLink linkToThisPage2 = TestEnvironment::firstResourceToThisPageOfType(pages[1], CrawlerEngine::ResourceType::ResourceHtml);

		EXPECT_EQ(QString(), linkToThisPage1.altOrTitle);
		EXPECT_EQ(CrawlerEngine::ResourceSource::SourceTagImg, linkToThisPage1.resourceSource);
		EXPECT_EQ(CrawlerEngine::LinkParameter::DofollowParameter, linkToThisPage1.linkParameter);
		EXPECT_EQ(0, pages[0]->linksOnThisPage.size());
		EXPECT_EQ(Common::StatusCode::NotFound404, pages[0]->statusCode);

		EXPECT_EQ(QString(), linkToThisPage2.altOrTitle);
		EXPECT_EQ(CrawlerEngine::ResourceSource::SourceTagImg, linkToThisPage2.resourceSource);
		EXPECT_EQ(CrawlerEngine::LinkParameter::DofollowParameter, linkToThisPage2.linkParameter);
		EXPECT_EQ(0, pages[1]->linksOnThisPage.size());
		EXPECT_EQ(Common::StatusCode::NotFound404, pages[1]->statusCode);

		const CrawlerEngine::ParsedPage* sourcePage1 = linkToThisPage1.resource.lock().get();
		const CrawlerEngine::ParsedPage* sourcePage2 = linkToThisPage2.resource.lock().get();

		CrawlerEngine::ResourceLink linkOnThisPage1 = TestEnvironment::firstResourceOnThisPageOfType(sourcePage1, CrawlerEngine::ResourceType::ResourceImage);
		CrawlerEngine::ResourceLink linkOnThisPage2 = TestEnvironment::firstResourceOnThisPageOfType(sourcePage2, CrawlerEngine::ResourceType::ResourceImage);

		EXPECT_EQ(QString(), linkOnThisPage1.altOrTitle);
		EXPECT_EQ(QString(), linkOnThisPage2.altOrTitle);

		EXPECT_EQ(CrawlerEngine::ResourceSource::SourceTagImg, linkOnThisPage1.resourceSource);
		EXPECT_EQ(CrawlerEngine::ResourceSource::SourceTagImg, linkOnThisPage2.resourceSource);


		auto pages404 = cl->waitForParsedPageReceived(CrawlerEngine::Status404StorageType, 2, 10, "Waiting for 2 404 status pages");
		EXPECT_EQ(2, pages404.size());

		auto pagesImage = cl->waitForParsedPageReceived(CrawlerEngine::ImageResourcesStorageType, 1, 10, "Waiting for 2 image pages");
		EXPECT_EQ(2, pagesImage.size());
	};

	env.initializeTest(testFunction);
	env.exec();
}

TEST(ImageTests, TwoPagesWithTheSameImage)
{
	int argc = 0;
	TestEnvironment env(TestEnvironment::defaultOptions(QUrl("http://image.com/two-pages-with-same-image-1.html")));

	const auto testFunction = [cl = env.crawler()]()
	{
		auto pages = cl->waitForParsedPageReceived(CrawlerEngine::CrawledUrlStorageType, 3, 10, "Waiting for 3 crawled pages");
		cl->checkSequencedDataCollectionConsistency();
		EXPECT_EQ(3, pages.size());

		auto images = cl->waitForParsedPageReceived(CrawlerEngine::ImageResourcesStorageType, 1, 10, "Waiting for 1 image page");
		auto changes = cl->waitForLinksToThisResourceChangesReceived(images[0], 2, 10);
		EXPECT_EQ(2, changes.size());
	};

	env.initializeTest(testFunction);
	env.exec();
}

TEST(ImageTests, ImageAlt)
{
	TestEnvironment env(TestEnvironment::defaultOptions(QUrl("http://image.com/image-alt.html")));

	const auto testFunction = [cl = env.crawler()]()
	{
		auto pages = cl->waitForAllCrawledPageReceived(10);
		cl->checkSequencedDataCollectionConsistency();
		EXPECT_EQ(1, pages.size());
		EXPECT_EQ(1, pages[0]->linksToThisPage.size());
		EXPECT_EQ(0, pages[0]->linksOnThisPage.size());
		EXPECT_EQ(Common::StatusCode::Ok200, pages[0]->statusCode);
		EXPECT_EQ(0, cl->storageItems(StorageType::MissingAltTextImageStorageType).size());

		QString expectedAlt = QString::fromWCharArray(L"Dummy Alt и чето по русски");
		EXPECT_EQ(expectedAlt, pages[0]->linksToThisPage[0].altOrTitle);
	};

	env.initializeTest(testFunction);
	env.exec();
}


TEST(ImageTests, TooBigImage)
{
	// too-big-image.html -> too-big-image-2.html
	CrawlerEngine::CrawlerOptions options = TestEnvironment::defaultOptions(QUrl("http://image.com/too-big-image.html"));
	options.maxImageSizeKb = 0;

	TestEnvironment env(options);

	const auto testFunction = [cl = env.crawler()]()
	{
		auto pages = cl->waitForParsedPageReceived(CrawlerEngine::Over100kbImageStorageType, 2, 100, "Waiting for 2 too big image page");
		cl->checkSequencedDataCollectionConsistency();

		EXPECT_EQ(2076, pages[0]->rawResponse.size());
		EXPECT_EQ(2076, pages[1]->rawResponse.size());

		cl->waitForParsedPageReceived(CrawlerEngine::CrawledUrlStorageType, 4, 10, "Waiting for 4 crawled pages");
	};

	env.initializeTest(testFunction);
	env.exec();
}

}
