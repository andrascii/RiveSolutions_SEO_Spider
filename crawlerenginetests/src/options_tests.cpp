namespace CrawlerEngineTests
{

using namespace CrawlerEngine;

TEST(OptionsTests, ParseOnlyHtml)
{
	CrawlerOptions options = TestEnvironment::defaultOptions({ Url("http://options.com/index.html") });
	options.parserTypeFlags = ParserTypeFlags();
	TestEnvironment env(options);

	const auto testFunction = [cl = env.crawler()]()
	{
		cl->waitForAllCrawledPageReceived(10);
		cl->checkSequencedDataCollectionConsistency();

		auto htmlResources = cl->storageItems(StorageType::HtmlResourcesStorageType);
		auto jsResources = cl->storageItems(StorageType::JavaScriptResourcesStorageType);
		auto cssResources = cl->storageItems(StorageType::StyleSheetResourcesStorageType);
		auto imageResources = cl->storageItems(StorageType::ImageResourcesStorageType);
		auto videoResources = cl->storageItems(StorageType::VideoResourcesStorageType);
		auto flashResources = cl->storageItems(StorageType::FlashResourcesStorageType);
		auto otherResources = cl->storageItems(StorageType::OtherResourcesStorageType);
		auto externalOtherResources = cl->storageItems(StorageType::ExternalOtherResourcesStorageType);

		EXPECT_EQ(2, htmlResources.size());
		EXPECT_EQ(0, jsResources.size());
		EXPECT_EQ(0, cssResources.size());
		EXPECT_EQ(0, imageResources.size());
		EXPECT_EQ(0, videoResources.size());
		EXPECT_EQ(0, flashResources.size());
		EXPECT_EQ(0, otherResources.size());
		EXPECT_EQ(0, externalOtherResources.size());
	};

	env.initializeTest(testFunction);
	env.exec();
}

TEST(OptionsTests, ParseOnlyJs)
{
	CrawlerOptions options = TestEnvironment::defaultOptions({ Url("http://options.com/index.html") });
	options.parserTypeFlags = JavaScriptResourcesParserType;
	TestEnvironment env(options);

	const auto testFunction = [cl = env.crawler()]()
	{
		cl->waitForAllCrawledPageReceived(10);
		cl->checkSequencedDataCollectionConsistency();

		auto htmlResources = cl->storageItems(StorageType::HtmlResourcesStorageType);
		auto jsResources = cl->storageItems(StorageType::JavaScriptResourcesStorageType);
		auto cssResources = cl->storageItems(StorageType::StyleSheetResourcesStorageType);
		auto imageResources = cl->storageItems(StorageType::ImageResourcesStorageType);
		auto videoResources = cl->storageItems(StorageType::VideoResourcesStorageType);
		auto flashResources = cl->storageItems(StorageType::FlashResourcesStorageType);
		auto otherResources = cl->storageItems(StorageType::OtherResourcesStorageType);
		auto externalOtherResources = cl->storageItems(StorageType::ExternalOtherResourcesStorageType);

		EXPECT_EQ(2, htmlResources.size());
		EXPECT_EQ(2, jsResources.size());
		EXPECT_EQ(0, cssResources.size());
		EXPECT_EQ(0, imageResources.size());
		EXPECT_EQ(0, videoResources.size());
		EXPECT_EQ(0, flashResources.size());
		EXPECT_EQ(0, otherResources.size());
		EXPECT_EQ(0, externalOtherResources.size());
	};

	env.initializeTest(testFunction);
	env.exec();
}

TEST(OptionsTests, ParseOnlyCSS)
{
	CrawlerOptions options = TestEnvironment::defaultOptions({ Url("http://options.com/index.html") });
	options.parserTypeFlags = CssResourcesParserType;
	TestEnvironment env(options);

	const auto testFunction = [cl = env.crawler()]()
	{
		cl->waitForAllCrawledPageReceived(10);
		cl->checkSequencedDataCollectionConsistency();

		auto htmlResources = cl->storageItems(StorageType::HtmlResourcesStorageType);
		auto jsResources = cl->storageItems(StorageType::JavaScriptResourcesStorageType);
		auto cssResources = cl->storageItems(StorageType::StyleSheetResourcesStorageType);
		auto imageResources = cl->storageItems(StorageType::ImageResourcesStorageType);
		auto videoResources = cl->storageItems(StorageType::VideoResourcesStorageType);
		auto flashResources = cl->storageItems(StorageType::FlashResourcesStorageType);
		auto otherResources = cl->storageItems(StorageType::OtherResourcesStorageType);
		auto externalOtherResources = cl->storageItems(StorageType::ExternalOtherResourcesStorageType);

		EXPECT_EQ(2, htmlResources.size());
		EXPECT_EQ(0, jsResources.size());
		EXPECT_EQ(2, cssResources.size());
		EXPECT_EQ(0, imageResources.size());
		EXPECT_EQ(0, videoResources.size());
		EXPECT_EQ(0, flashResources.size());
		EXPECT_EQ(0, otherResources.size());
		EXPECT_EQ(0, externalOtherResources.size());

	};

	env.initializeTest(testFunction);
	env.exec();
}

TEST(OptionsTests, ParseOnlyImages)
{
	CrawlerOptions options = TestEnvironment::defaultOptions({ Url("http://options.com/index.html") });
	options.parserTypeFlags = ImagesResourcesParserType;
	TestEnvironment env(options);

	const auto testFunction = [cl = env.crawler()]()
	{
		cl->waitForAllCrawledPageReceived(10);
		cl->checkSequencedDataCollectionConsistency();

		auto htmlResources = cl->storageItems(StorageType::HtmlResourcesStorageType);
		auto jsResources = cl->storageItems(StorageType::JavaScriptResourcesStorageType);
		auto cssResources = cl->storageItems(StorageType::StyleSheetResourcesStorageType);
		auto imageResources = cl->storageItems(StorageType::ImageResourcesStorageType);
		auto videoResources = cl->storageItems(StorageType::VideoResourcesStorageType);
		auto flashResources = cl->storageItems(StorageType::FlashResourcesStorageType);
		auto otherResources = cl->storageItems(StorageType::OtherResourcesStorageType);
		auto externalOtherResources = cl->storageItems(StorageType::ExternalOtherResourcesStorageType);

		EXPECT_EQ(2, htmlResources.size());
		EXPECT_EQ(0, jsResources.size());
		EXPECT_EQ(0, cssResources.size());
		EXPECT_EQ(2, imageResources.size());
		EXPECT_EQ(0, videoResources.size());
		EXPECT_EQ(0, flashResources.size());
		EXPECT_EQ(0, otherResources.size());
		EXPECT_EQ(0, externalOtherResources.size());
	};

	env.initializeTest(testFunction);
	env.exec();
}

TEST(OptionsTests, ParseOnlyVideo)
{
	CrawlerOptions options = TestEnvironment::defaultOptions({ Url("http://options.com/index.html") });
	options.parserTypeFlags = VideoResourcesParserType;
	TestEnvironment env(options);

	const auto testFunction = [cl = env.crawler()]()
	{
		cl->waitForAllCrawledPageReceived(10);
		cl->checkSequencedDataCollectionConsistency();

		auto htmlResources = cl->storageItems(StorageType::HtmlResourcesStorageType);
		auto jsResources = cl->storageItems(StorageType::JavaScriptResourcesStorageType);
		auto cssResources = cl->storageItems(StorageType::StyleSheetResourcesStorageType);
		auto imageResources = cl->storageItems(StorageType::ImageResourcesStorageType);
		auto videoResources = cl->storageItems(StorageType::VideoResourcesStorageType);
		auto flashResources = cl->storageItems(StorageType::FlashResourcesStorageType);
		auto otherResources = cl->storageItems(StorageType::OtherResourcesStorageType);
		auto externalOtherResources = cl->storageItems(StorageType::ExternalOtherResourcesStorageType);

		EXPECT_EQ(2, htmlResources.size());
		EXPECT_EQ(0, jsResources.size());
		EXPECT_EQ(0, cssResources.size());
		EXPECT_EQ(0, imageResources.size());
		EXPECT_EQ(2, videoResources.size());
		EXPECT_EQ(0, flashResources.size());
		EXPECT_EQ(0, otherResources.size());
		EXPECT_EQ(0, externalOtherResources.size());
	};

	env.initializeTest(testFunction);
	env.exec();
}

TEST(OptionsTests, ParseOnlyFlash)
{
	CrawlerOptions options = TestEnvironment::defaultOptions({ Url("http://options.com/index.html") });
	options.parserTypeFlags = FlashResourcesParserType;
	TestEnvironment env(options);

	const auto testFunction = [cl = env.crawler()]()
	{
		cl->waitForAllCrawledPageReceived(10);
		cl->checkSequencedDataCollectionConsistency();

		auto htmlResources = cl->storageItems(StorageType::HtmlResourcesStorageType);
		auto jsResources = cl->storageItems(StorageType::JavaScriptResourcesStorageType);
		auto cssResources = cl->storageItems(StorageType::StyleSheetResourcesStorageType);
		auto imageResources = cl->storageItems(StorageType::ImageResourcesStorageType);
		auto videoResources = cl->storageItems(StorageType::VideoResourcesStorageType);
		auto flashResources = cl->storageItems(StorageType::FlashResourcesStorageType);
		auto otherResources = cl->storageItems(StorageType::OtherResourcesStorageType);
		auto externalOtherResources = cl->storageItems(StorageType::ExternalOtherResourcesStorageType);

		EXPECT_EQ(2, htmlResources.size());
		EXPECT_EQ(0, jsResources.size());
		EXPECT_EQ(0, cssResources.size());
		EXPECT_EQ(0, imageResources.size());
		EXPECT_EQ(0, videoResources.size());
		EXPECT_EQ(2, flashResources.size());
		EXPECT_EQ(0, otherResources.size());
		EXPECT_EQ(0, externalOtherResources.size());
	};

	env.initializeTest(testFunction);
	env.exec();
}

TEST(OptionsTests, ParseOnlyOther)
{
	CrawlerOptions options = TestEnvironment::defaultOptions({ Url("http://options.com/index.html") });
	options.parserTypeFlags = OtherResourcesParserType;
	TestEnvironment env(options);

	const auto testFunction = [cl = env.crawler()]()
	{
		cl->waitForAllCrawledPageReceived(10);
		cl->checkSequencedDataCollectionConsistency();

		auto htmlResources = cl->storageItems(StorageType::HtmlResourcesStorageType);
		auto jsResources = cl->storageItems(StorageType::JavaScriptResourcesStorageType);
		auto cssResources = cl->storageItems(StorageType::StyleSheetResourcesStorageType);
		auto imageResources = cl->storageItems(StorageType::ImageResourcesStorageType);
		auto videoResources = cl->storageItems(StorageType::VideoResourcesStorageType);
		auto flashResources = cl->storageItems(StorageType::FlashResourcesStorageType);
		auto otherResources = cl->storageItems(StorageType::OtherResourcesStorageType);
		auto externalOtherResources = cl->storageItems(StorageType::ExternalOtherResourcesStorageType);

		EXPECT_EQ(2, htmlResources.size());
		EXPECT_EQ(0, jsResources.size());
		EXPECT_EQ(0, cssResources.size());
		EXPECT_EQ(0, imageResources.size());
		EXPECT_EQ(0, videoResources.size());
		EXPECT_EQ(0, flashResources.size());
		EXPECT_EQ(2, otherResources.size());
		EXPECT_EQ(1, externalOtherResources.size());
	};

	env.initializeTest(testFunction);
	env.exec();
}

}