namespace CrawlerEngineTests
{

TEST(OptionsTests, ParseOnlyHtml)
{
	std::lock_guard<std::mutex> locker(g_mutex);

	CrawlerEngine::CrawlerOptions options = TestEnvironment::defaultOptions({ QUrl("http://options.com/index.html") });
	options.parserTypeFlags = CrawlerEngine::ParserTypeFlags();
	TestEnvironment env(options);

	const auto testFunction = [cl = env.crawler()]()
	{
		//
		// TODO: Add signal from crawler about crawwling ended
		//

		testEnv->thread()->msleep(500);
		//cl->checkSequencedDataCollectionConsistency();

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
	};

	env.initializeTest(testFunction);
	env.exec();
}

TEST(OptionsTests, ParseOnlyJs)
{
	std::lock_guard<std::mutex> locker(g_mutex);

	CrawlerEngine::CrawlerOptions options = TestEnvironment::defaultOptions({ QUrl("http://options.com/index.html") });
	options.parserTypeFlags = CrawlerEngine::JavaScriptResourcesParserType;
	TestEnvironment env(options);

	const auto testFunction = [cl = env.crawler()]()
	{
		//
		// TODO: Add signal from crawler about crawwling ended
		//

		testEnv->thread()->msleep(500);
		//cl->checkSequencedDataCollectionConsistency();

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
	};

	env.initializeTest(testFunction);
	env.exec();
}

TEST(OptionsTests, ParseOnlyCSS)
{
	std::lock_guard<std::mutex> locker(g_mutex);

	CrawlerEngine::CrawlerOptions options = TestEnvironment::defaultOptions({ QUrl("http://options.com/index.html") });
	options.parserTypeFlags = CrawlerEngine::CssResourcesParserType;
	TestEnvironment env(options);

	const auto testFunction = [cl = env.crawler()]()
	{
		//
		// TODO: Add signal from crawler about crawwling ended
		//

		testEnv->thread()->msleep(500);
		//cl->checkSequencedDataCollectionConsistency();

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

	};

	env.initializeTest(testFunction);
	env.exec();
}

TEST(OptionsTests, ParseOnlyImages)
{
	std::lock_guard<std::mutex> locker(g_mutex);

	CrawlerEngine::CrawlerOptions options = TestEnvironment::defaultOptions({ QUrl("http://options.com/index.html") });
	options.parserTypeFlags = CrawlerEngine::ImagesResourcesParserType;
	TestEnvironment env(options);

	const auto testFunction = [cl = env.crawler()]()
	{
		//
		// TODO: Add signal from crawler about crawwling ended
		//

		testEnv->thread()->msleep(500);
		//cl->checkSequencedDataCollectionConsistency();

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
	};

	env.initializeTest(testFunction);
	env.exec();
}

TEST(OptionsTests, ParseOnlyVideo)
{
	std::lock_guard<std::mutex> locker(g_mutex);

	CrawlerEngine::CrawlerOptions options = TestEnvironment::defaultOptions({ QUrl("http://options.com/index.html") });
	options.parserTypeFlags = CrawlerEngine::VideoResourcesParserType;
	TestEnvironment env(options);

	const auto testFunction = [cl = env.crawler()]()
	{
		//
		// TODO: Add signal from crawler about crawwling ended
		//

		testEnv->thread()->msleep(500);
		//cl->checkSequencedDataCollectionConsistency();

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
	};

	env.initializeTest(testFunction);
	env.exec();
}

TEST(OptionsTests, ParseOnlyFlash)
{
	std::lock_guard<std::mutex> locker(g_mutex);

	CrawlerEngine::CrawlerOptions options = TestEnvironment::defaultOptions({ QUrl("http://options.com/index.html") });
	options.parserTypeFlags = CrawlerEngine::FlashResourcesParserType;
	TestEnvironment env(options);

	const auto testFunction = [cl = env.crawler()]()
	{
		//
		// TODO: Add signal from crawler about crawling ended
		//

		testEnv->thread()->msleep(500);
		//cl->checkSequencedDataCollectionConsistency();

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
	};

	env.initializeTest(testFunction);
	env.exec();
}

TEST(OptionsTests, ParseOnlyOther)
{
	std::lock_guard<std::mutex> locker(g_mutex);

	CrawlerEngine::CrawlerOptions options = TestEnvironment::defaultOptions({ QUrl("http://options.com/index.html") });
	options.parserTypeFlags = CrawlerEngine::OtherResourcesParserType;
	TestEnvironment env(options);

	const auto testFunction = [cl = env.crawler()]()
	{
		//
		// TODO: Add signal from crawler about crawling ended
		//

		testEnv->thread()->msleep(300);
		//cl->checkSequencedDataCollectionConsistency();

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
	};

	env.initializeTest(testFunction);
	env.exec();
}

}