namespace CrawlerEngineTests
{

TEST(OptionsTests, ParseOnlyHtml)
{
	auto options = TestEnvironment::defaultOptions({ QUrl("http://options.com/index.html") });
	options.parserTypeFlags = CrawlerEngine::ParserTypeFlags();
		
	TestEnvironment env(options);
	env.runTest([cl = env.crawler()]()
	{
		cl->thread()->msleep(800);

		auto htmlResources = cl->storageItems(CrawlerEngine::HtmlResourcesStorageType);
		auto jsResources = cl->storageItems(CrawlerEngine::JavaScriptResourcesStorageType);
		auto cssResources = cl->storageItems(CrawlerEngine::StyleSheetResourcesStorageType);
		auto imageResources = cl->storageItems(CrawlerEngine::ImageResourcesStorageType);
		auto videoResources = cl->storageItems(CrawlerEngine::VideoResourcesStorageType);
		auto flashResources = cl->storageItems(CrawlerEngine::FlashResourcesStorageType);
		auto otherResources = cl->storageItems(CrawlerEngine::OtherResourcesStorageType);
		auto externalOtherResources = cl->storageItems(CrawlerEngine::ExternalOtherResourcesStorageType);

		EXPECT_EQ(2, htmlResources.size());
		EXPECT_EQ(0, jsResources.size());
		EXPECT_EQ(0, cssResources.size());
		EXPECT_EQ(0, imageResources.size());
		EXPECT_EQ(0, videoResources.size());
		EXPECT_EQ(0, flashResources.size());
		EXPECT_EQ(0, otherResources.size());
		EXPECT_EQ(0, externalOtherResources.size());
	});
}

TEST(OptionsTests, ParseOnlyJs)
{
	auto options = TestEnvironment::defaultOptions({ QUrl("http://options.com/index.html") });
	options.parserTypeFlags = CrawlerEngine::JavaScriptResourcesParserType;

	TestEnvironment env(options);
	env.runTest([cl = env.crawler()]()
	{
		cl->thread()->msleep(300);

		auto htmlResources = cl->storageItems(CrawlerEngine::HtmlResourcesStorageType);
		auto jsResources = cl->storageItems(CrawlerEngine::JavaScriptResourcesStorageType);
		auto cssResources = cl->storageItems(CrawlerEngine::StyleSheetResourcesStorageType);
		auto imageResources = cl->storageItems(CrawlerEngine::ImageResourcesStorageType);
		auto videoResources = cl->storageItems(CrawlerEngine::VideoResourcesStorageType);
		auto flashResources = cl->storageItems(CrawlerEngine::FlashResourcesStorageType);
		auto otherResources = cl->storageItems(CrawlerEngine::OtherResourcesStorageType);
		auto externalOtherResources = cl->storageItems(CrawlerEngine::ExternalOtherResourcesStorageType);
		
		EXPECT_EQ(2, htmlResources.size());
		EXPECT_EQ(2, jsResources.size());
		EXPECT_EQ(0, cssResources.size());
		EXPECT_EQ(0, imageResources.size());
		EXPECT_EQ(0, videoResources.size());
		EXPECT_EQ(0, flashResources.size());
		EXPECT_EQ(0, otherResources.size());
		EXPECT_EQ(0, externalOtherResources.size());

	});
}

TEST(OptionsTests, ParseOnlyCSS)
{
	auto options = TestEnvironment::defaultOptions({ QUrl("http://options.com/index.html") });
	options.parserTypeFlags = CrawlerEngine::CssResourcesParserType;

	TestEnvironment env(options);
	env.runTest([cl = env.crawler()]()
	{
		cl->thread()->msleep(300);

		auto htmlResources = cl->storageItems(CrawlerEngine::HtmlResourcesStorageType);
		auto jsResources = cl->storageItems(CrawlerEngine::JavaScriptResourcesStorageType);
		auto cssResources = cl->storageItems(CrawlerEngine::StyleSheetResourcesStorageType);
		auto imageResources = cl->storageItems(CrawlerEngine::ImageResourcesStorageType);
		auto videoResources = cl->storageItems(CrawlerEngine::VideoResourcesStorageType);
		auto flashResources = cl->storageItems(CrawlerEngine::FlashResourcesStorageType);
		auto otherResources = cl->storageItems(CrawlerEngine::OtherResourcesStorageType);
		auto externalOtherResources = cl->storageItems(CrawlerEngine::ExternalOtherResourcesStorageType);

		EXPECT_EQ(2, htmlResources.size());
		EXPECT_EQ(0, jsResources.size());
		EXPECT_EQ(2, cssResources.size());
		EXPECT_EQ(0, imageResources.size());
		EXPECT_EQ(0, videoResources.size());
		EXPECT_EQ(0, flashResources.size());
		EXPECT_EQ(0, otherResources.size());
		EXPECT_EQ(0, externalOtherResources.size());

	});
}

TEST(OptionsTests, ParseOnlyImages)
{
	auto options = TestEnvironment::defaultOptions({ QUrl("http://options.com/index.html") });
	options.parserTypeFlags = CrawlerEngine::ImagesResourcesParserType;

	TestEnvironment env(options);
	env.runTest([cl = env.crawler()]()
	{
		cl->thread()->msleep(300);

		auto htmlResources = cl->storageItems(CrawlerEngine::HtmlResourcesStorageType);
		auto jsResources = cl->storageItems(CrawlerEngine::JavaScriptResourcesStorageType);
		auto cssResources = cl->storageItems(CrawlerEngine::StyleSheetResourcesStorageType);
		auto imageResources = cl->storageItems(CrawlerEngine::ImageResourcesStorageType);
		auto videoResources = cl->storageItems(CrawlerEngine::VideoResourcesStorageType);
		auto flashResources = cl->storageItems(CrawlerEngine::FlashResourcesStorageType);
		auto otherResources = cl->storageItems(CrawlerEngine::OtherResourcesStorageType);
		auto externalOtherResources = cl->storageItems(CrawlerEngine::ExternalOtherResourcesStorageType);

		EXPECT_EQ(2, htmlResources.size());
		EXPECT_EQ(0, jsResources.size());
		EXPECT_EQ(0, cssResources.size());
		EXPECT_EQ(2, imageResources.size());
		EXPECT_EQ(0, videoResources.size());
		EXPECT_EQ(0, flashResources.size());
		EXPECT_EQ(0, otherResources.size());
		EXPECT_EQ(0, externalOtherResources.size());
	});
}

TEST(OptionsTests, ParseOnlyVideo)
{
	auto options = TestEnvironment::defaultOptions({ QUrl("http://options.com/index.html") });
	options.parserTypeFlags = CrawlerEngine::VideoResourcesParserType;

	TestEnvironment env(options);
	env.runTest([cl = env.crawler()]()
	{
		cl->thread()->msleep(300);

		auto htmlResources = cl->storageItems(CrawlerEngine::HtmlResourcesStorageType);
		auto jsResources = cl->storageItems(CrawlerEngine::JavaScriptResourcesStorageType);
		auto cssResources = cl->storageItems(CrawlerEngine::StyleSheetResourcesStorageType);
		auto imageResources = cl->storageItems(CrawlerEngine::ImageResourcesStorageType);
		auto videoResources = cl->storageItems(CrawlerEngine::VideoResourcesStorageType);
		auto flashResources = cl->storageItems(CrawlerEngine::FlashResourcesStorageType);
		auto otherResources = cl->storageItems(CrawlerEngine::OtherResourcesStorageType);
		auto externalOtherResources = cl->storageItems(CrawlerEngine::ExternalOtherResourcesStorageType);

		EXPECT_EQ(2, htmlResources.size());
		EXPECT_EQ(0, jsResources.size());
		EXPECT_EQ(0, cssResources.size());
		EXPECT_EQ(0, imageResources.size());
		EXPECT_EQ(2, videoResources.size());
		EXPECT_EQ(0, flashResources.size());
		EXPECT_EQ(0, otherResources.size());
		EXPECT_EQ(0, externalOtherResources.size());
	});
}

TEST(OptionsTests, ParseOnlyFlash)
{
	auto options = TestEnvironment::defaultOptions({ QUrl("http://options.com/index.html") });
	options.parserTypeFlags = CrawlerEngine::FlashResourcesParserType;

	TestEnvironment env(options);
	env.runTest([cl = env.crawler()]()
	{
		cl->thread()->msleep(300);

		auto htmlResources = cl->storageItems(CrawlerEngine::HtmlResourcesStorageType);
		auto jsResources = cl->storageItems(CrawlerEngine::JavaScriptResourcesStorageType);
		auto cssResources = cl->storageItems(CrawlerEngine::StyleSheetResourcesStorageType);
		auto imageResources = cl->storageItems(CrawlerEngine::ImageResourcesStorageType);
		auto videoResources = cl->storageItems(CrawlerEngine::VideoResourcesStorageType);
		auto flashResources = cl->storageItems(CrawlerEngine::FlashResourcesStorageType);
		auto otherResources = cl->storageItems(CrawlerEngine::OtherResourcesStorageType);
		auto externalOtherResources = cl->storageItems(CrawlerEngine::ExternalOtherResourcesStorageType);

		EXPECT_EQ(2, htmlResources.size());
		EXPECT_EQ(0, jsResources.size());
		EXPECT_EQ(0, cssResources.size());
		EXPECT_EQ(0, imageResources.size());
		EXPECT_EQ(0, videoResources.size());
		EXPECT_EQ(2, flashResources.size());
		EXPECT_EQ(0, otherResources.size());
		EXPECT_EQ(0, externalOtherResources.size());
	});
}

TEST(OptionsTests, ParseOnlyOther)
{
	auto options = TestEnvironment::defaultOptions({ QUrl("http://options.com/index.html") });
	options.parserTypeFlags = CrawlerEngine::OtherResourcesParserType;

	TestEnvironment env(options);
	env.runTest([cl = env.crawler()]()
	{
		cl->thread()->msleep(300);

		auto htmlResources = cl->storageItems(CrawlerEngine::HtmlResourcesStorageType);
		auto jsResources = cl->storageItems(CrawlerEngine::JavaScriptResourcesStorageType);
		auto cssResources = cl->storageItems(CrawlerEngine::StyleSheetResourcesStorageType);
		auto imageResources = cl->storageItems(CrawlerEngine::ImageResourcesStorageType);
		auto videoResources = cl->storageItems(CrawlerEngine::VideoResourcesStorageType);
		auto flashResources = cl->storageItems(CrawlerEngine::FlashResourcesStorageType);
		auto otherResources = cl->storageItems(CrawlerEngine::OtherResourcesStorageType);
		auto externalOtherResources = cl->storageItems(CrawlerEngine::ExternalOtherResourcesStorageType);

		EXPECT_EQ(2, htmlResources.size());
		EXPECT_EQ(0, jsResources.size());
		EXPECT_EQ(0, cssResources.size());
		EXPECT_EQ(0, imageResources.size());
		EXPECT_EQ(0, videoResources.size());
		EXPECT_EQ(0, flashResources.size());
		EXPECT_EQ(2, otherResources.size());
		EXPECT_EQ(1, externalOtherResources.size());
	});
}

}