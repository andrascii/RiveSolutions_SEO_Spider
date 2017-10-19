namespace WebCrawlerTests
{

TEST(OptionsTests, ParseOnlyHtml)
{
	auto options = TestEnvironment::defaultOptions({ QUrl("http://options.com/index.html") });
		
	TestEnvironment env(options);
	env.runTest([cl = env.crawler()]()
	{
		cl->thread()->msleep(800);

		auto htmlResources = cl->storageItems(WebCrawler::HtmlResourcesStorageType);
		EXPECT_EQ(2, htmlResources.size());

		auto jsResources = cl->storageItems(WebCrawler::JavaScriptResourcesStorageType);
		EXPECT_EQ(0, jsResources.size());

		auto cssResources = cl->storageItems(WebCrawler::StyleSheetResourcesStorageType);
		EXPECT_EQ(0, cssResources.size());

		auto imageResources = cl->storageItems(WebCrawler::ImageResourcesStorageType);
		EXPECT_EQ(0, imageResources.size());

		auto videoResources = cl->storageItems(WebCrawler::VideoResourcesStorageType);
		EXPECT_EQ(0, videoResources.size());

		auto flashResources = cl->storageItems(WebCrawler::FlashResourcesStorageType);
		EXPECT_EQ(0, flashResources.size());

		// TODO: fix

		//auto otherResources = cl->storageItems(WebCrawler::OtherResourcesStorageType);
		//EXPECT_EQ(0, otherResources.size());

	});
}

TEST(OptionsTests, ParseOnlyJs)
{
	auto options = TestEnvironment::defaultOptions({ QUrl("http://options.com/index.html") });
	options.parserTypeFlags = WebCrawler::JavaScriptResourcesParserType;

	TestEnvironment env(options);
	env.runTest([cl = env.crawler()]()
	{
		cl->thread()->msleep(300);

		// TODO: fix

		//auto htmlResources = cl->storageItems(WebCrawler::HtmlResourcesStorageType);
		//EXPECT_EQ(0, htmlResources.size());

		auto jsResources = cl->storageItems(WebCrawler::JavaScriptResourcesStorageType);
		EXPECT_EQ(2, jsResources.size());

		auto cssResources = cl->storageItems(WebCrawler::StyleSheetResourcesStorageType);
		EXPECT_EQ(0, cssResources.size());

		auto imageResources = cl->storageItems(WebCrawler::ImageResourcesStorageType);
		EXPECT_EQ(0, imageResources.size());

		auto videoResources = cl->storageItems(WebCrawler::VideoResourcesStorageType);
		EXPECT_EQ(0, videoResources.size());

		auto flashResources = cl->storageItems(WebCrawler::FlashResourcesStorageType);
		EXPECT_EQ(0, flashResources.size());

		// TODO: fix

		//auto otherResources = cl->storageItems(WebCrawler::OtherResourcesStorageType);
		//EXPECT_EQ(0, otherResources.size());

	});
}

}