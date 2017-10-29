namespace WebCrawlerTests
{

TEST(OptionsTests, ParseOnlyHtml)
{
	auto options = TestEnvironment::defaultOptions({ QUrl("http://options.com/index.html") });
	options.parserTypeFlags = WebCrawler::ParserTypeFlags();
		
	TestEnvironment env(options);
	env.runTest([cl = env.crawler()]()
	{
		cl->thread()->msleep(800);

		auto htmlResources = cl->storageItems(WebCrawler::HtmlResourcesStorageType);
		auto jsResources = cl->storageItems(WebCrawler::JavaScriptResourcesStorageType);
		auto cssResources = cl->storageItems(WebCrawler::StyleSheetResourcesStorageType);
		auto imageResources = cl->storageItems(WebCrawler::ImageResourcesStorageType);
		auto videoResources = cl->storageItems(WebCrawler::VideoResourcesStorageType);
		auto flashResources = cl->storageItems(WebCrawler::FlashResourcesStorageType);
		auto otherResources = cl->storageItems(WebCrawler::OtherResourcesStorageType);
		auto externalOtherResources = cl->storageItems(WebCrawler::ExternalOtherResourcesStorageType);

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
	options.parserTypeFlags = WebCrawler::JavaScriptResourcesParserType;

	TestEnvironment env(options);
	env.runTest([cl = env.crawler()]()
	{
		cl->thread()->msleep(300);

		auto htmlResources = cl->storageItems(WebCrawler::HtmlResourcesStorageType);
		auto jsResources = cl->storageItems(WebCrawler::JavaScriptResourcesStorageType);
		auto cssResources = cl->storageItems(WebCrawler::StyleSheetResourcesStorageType);
		auto imageResources = cl->storageItems(WebCrawler::ImageResourcesStorageType);
		auto videoResources = cl->storageItems(WebCrawler::VideoResourcesStorageType);
		auto flashResources = cl->storageItems(WebCrawler::FlashResourcesStorageType);
		auto otherResources = cl->storageItems(WebCrawler::OtherResourcesStorageType);
		auto externalOtherResources = cl->storageItems(WebCrawler::ExternalOtherResourcesStorageType);
		
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
	options.parserTypeFlags = WebCrawler::CssResourcesParserType;

	TestEnvironment env(options);
	env.runTest([cl = env.crawler()]()
	{
		cl->thread()->msleep(300);

		auto htmlResources = cl->storageItems(WebCrawler::HtmlResourcesStorageType);
		auto jsResources = cl->storageItems(WebCrawler::JavaScriptResourcesStorageType);
		auto cssResources = cl->storageItems(WebCrawler::StyleSheetResourcesStorageType);
		auto imageResources = cl->storageItems(WebCrawler::ImageResourcesStorageType);
		auto videoResources = cl->storageItems(WebCrawler::VideoResourcesStorageType);
		auto flashResources = cl->storageItems(WebCrawler::FlashResourcesStorageType);
		auto otherResources = cl->storageItems(WebCrawler::OtherResourcesStorageType);
		auto externalOtherResources = cl->storageItems(WebCrawler::ExternalOtherResourcesStorageType);

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
	options.parserTypeFlags = WebCrawler::ImagesResourcesParserType;

	TestEnvironment env(options);
	env.runTest([cl = env.crawler()]()
	{
		cl->thread()->msleep(300);

		auto htmlResources = cl->storageItems(WebCrawler::HtmlResourcesStorageType);
		auto jsResources = cl->storageItems(WebCrawler::JavaScriptResourcesStorageType);
		auto cssResources = cl->storageItems(WebCrawler::StyleSheetResourcesStorageType);
		auto imageResources = cl->storageItems(WebCrawler::ImageResourcesStorageType);
		auto videoResources = cl->storageItems(WebCrawler::VideoResourcesStorageType);
		auto flashResources = cl->storageItems(WebCrawler::FlashResourcesStorageType);
		auto otherResources = cl->storageItems(WebCrawler::OtherResourcesStorageType);
		auto externalOtherResources = cl->storageItems(WebCrawler::ExternalOtherResourcesStorageType);

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
	options.parserTypeFlags = WebCrawler::VideoResourcesParserType;

	TestEnvironment env(options);
	env.runTest([cl = env.crawler()]()
	{
		cl->thread()->msleep(300);

		auto htmlResources = cl->storageItems(WebCrawler::HtmlResourcesStorageType);
		auto jsResources = cl->storageItems(WebCrawler::JavaScriptResourcesStorageType);
		auto cssResources = cl->storageItems(WebCrawler::StyleSheetResourcesStorageType);
		auto imageResources = cl->storageItems(WebCrawler::ImageResourcesStorageType);
		auto videoResources = cl->storageItems(WebCrawler::VideoResourcesStorageType);
		auto flashResources = cl->storageItems(WebCrawler::FlashResourcesStorageType);
		auto otherResources = cl->storageItems(WebCrawler::OtherResourcesStorageType);
		auto externalOtherResources = cl->storageItems(WebCrawler::ExternalOtherResourcesStorageType);

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
	options.parserTypeFlags = WebCrawler::FlashResourcesParserType;

	TestEnvironment env(options);
	env.runTest([cl = env.crawler()]()
	{
		cl->thread()->msleep(300);

		auto htmlResources = cl->storageItems(WebCrawler::HtmlResourcesStorageType);
		auto jsResources = cl->storageItems(WebCrawler::JavaScriptResourcesStorageType);
		auto cssResources = cl->storageItems(WebCrawler::StyleSheetResourcesStorageType);
		auto imageResources = cl->storageItems(WebCrawler::ImageResourcesStorageType);
		auto videoResources = cl->storageItems(WebCrawler::VideoResourcesStorageType);
		auto flashResources = cl->storageItems(WebCrawler::FlashResourcesStorageType);
		auto otherResources = cl->storageItems(WebCrawler::OtherResourcesStorageType);
		auto externalOtherResources = cl->storageItems(WebCrawler::ExternalOtherResourcesStorageType);

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
	options.parserTypeFlags = WebCrawler::OtherResourcesParserType;

	TestEnvironment env(options);
	env.runTest([cl = env.crawler()]()
	{
		cl->thread()->msleep(300);

		auto htmlResources = cl->storageItems(WebCrawler::HtmlResourcesStorageType);
		auto jsResources = cl->storageItems(WebCrawler::JavaScriptResourcesStorageType);
		auto cssResources = cl->storageItems(WebCrawler::StyleSheetResourcesStorageType);
		auto imageResources = cl->storageItems(WebCrawler::ImageResourcesStorageType);
		auto videoResources = cl->storageItems(WebCrawler::VideoResourcesStorageType);
		auto flashResources = cl->storageItems(WebCrawler::FlashResourcesStorageType);
		auto otherResources = cl->storageItems(WebCrawler::OtherResourcesStorageType);
		auto externalOtherResources = cl->storageItems(WebCrawler::ExternalOtherResourcesStorageType);

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