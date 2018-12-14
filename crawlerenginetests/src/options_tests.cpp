namespace CrawlerEngineTests
{

using namespace CrawlerEngine;

TEST(OptionsTests, ParseOnlyHtml)
{
	TestEnvironment env;

	auto options = TestEnvironment::defaultOptions({ Url("http://options.com/index.html") });
	options.parserTypeFlags = ParserTypeFlags();
	
	env.crawler()->options()->setData(options);

	const auto testFunction = [cl = env.crawler()]()
	{
		cl->waitForAllCrawledPageReceived(10);

		// it fails because we don't loading "other resources", but this method checks
		// if zip resource is in CrawledStorageType
		//cl->checkSequencedDataCollectionConsistency();

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
	TestEnvironment env;

	auto options = TestEnvironment::defaultOptions({ Url("http://options.com/index.html") });
	options.parserTypeFlags = JavaScriptResourcesParserType;

	env.crawler()->options()->setData(options);

	const auto testFunction = [cl = env.crawler()]()
	{
		cl->waitForAllCrawledPageReceived(10);
		//cl->checkSequencedDataCollectionConsistency();

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
	TestEnvironment env;

	auto options = TestEnvironment::defaultOptions({ Url("http://options.com/index.html") });
	options.parserTypeFlags = CssResourcesParserType;

	env.crawler()->options()->setData(options);

	const auto testFunction = [cl = env.crawler()]()
	{
		cl->waitForAllCrawledPageReceived(10);
		//cl->checkSequencedDataCollectionConsistency();

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
	TestEnvironment env;

	auto options = TestEnvironment::defaultOptions({ Url("http://options.com/index.html") });
	options.parserTypeFlags = ImagesResourcesParserType;
	env.crawler()->options()->setData(options);

	const auto testFunction = [cl = env.crawler()]()
	{
		cl->waitForAllCrawledPageReceived(10);
		//cl->checkSequencedDataCollectionConsistency();

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
	TestEnvironment env;

	auto options = TestEnvironment::defaultOptions({ Url("http://options.com/index.html") });
	options.parserTypeFlags = VideoResourcesParserType;

	env.crawler()->options()->setData(options);

	const auto testFunction = [cl = env.crawler()]()
	{
		cl->waitForAllCrawledPageReceived(10);
		//cl->checkSequencedDataCollectionConsistency();

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
	TestEnvironment env;

	auto options = TestEnvironment::defaultOptions({ Url("http://options.com/index.html") });
	options.parserTypeFlags = FlashResourcesParserType;

	env.crawler()->options()->setData(options);

	const auto testFunction = [cl = env.crawler()]()
	{
		cl->waitForCrawlingDone();
		//cl->checkSequencedDataCollectionConsistency();

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
	TestEnvironment env;

	auto options = TestEnvironment::defaultOptions({ Url("http://options.com/index.html") });
	options.parserTypeFlags = OtherResourcesParserType;
	options.checkExternalLinks = true;

	env.crawler()->options()->setData(options);

	const auto testFunction = [cl = env.crawler()]()
	{
		cl->waitForAllCrawledPageReceived(10);
		//cl->checkSequencedDataCollectionConsistency();

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

TEST(OptionsTests, BlockedByRobotsTxtLinksMustNotBeLoaded)
{
	TestEnvironment env;

	auto options = TestEnvironment::defaultOptions(Url("http://blockedbyrobotstxt.com"));
	options.followRobotsTxtRules = true;
	options.userAgentToFollow = UserAgentType::AnyBot;

	env.crawler()->options()->setData(options);

	const auto testFunction = [cl = env.crawler()]()
	{
		auto pages = cl->waitForAllCrawledPageReceived(10);
		//cl->checkSequencedDataCollectionConsistency();

		std::vector<const ParsedPage*> crawledPages = cl->storageItems(StorageType::CrawledUrlStorageType);
		std::vector<const ParsedPage*> pendingPages = cl->getLinksFromUnorderedDataCollection(StorageType::PendingResourcesStorageType);

		const Url blockedByRobotsUrl("http://blockedbyrobotstxt.com/blocked.html");

		for (std::size_t i = 0; i < crawledPages.size(); ++i)
		{
			if (crawledPages[i]->url != blockedByRobotsUrl)
			{
				continue;
			}

			EXPECT_EQ(true, crawledPages[i]->statusCode == Common::StatusCode::BlockedByRobotsTxt);
		}

		const auto findIterator = std::find_if(pendingPages.begin(), pendingPages.end(),
			[&blockedByRobotsUrl](const ParsedPage* page) { return page->url == blockedByRobotsUrl; });

		EXPECT_EQ(true, findIterator == pendingPages.end());
	};

	env.initializeTest(testFunction);
	env.exec();
}

TEST(OptionsTests, CheckExternalLinkOnlyOption)
{
	TestEnvironment env;

	auto options = TestEnvironment::defaultOptions({ Url("http://options.com/index.html") });
	options.checkExternalLinks = true;

	env.crawler()->options()->setData(options);

	const auto testFunction = [cl = env.crawler()]()
	{
		cl->waitForAllCrawledPageReceived(10);
		//cl->checkSequencedDataCollectionConsistency();

		std::vector<const ParsedPage*> crawledPages = cl->storageItems(StorageType::CrawledUrlStorageType);

		const Url externalLink("http://justanothersite.com");

		const auto findIterator = std::find_if(crawledPages.begin(), crawledPages.end(),
			[&](const ParsedPage* page) { return page->url == externalLink; });

		EXPECT_EQ(true, findIterator != crawledPages.end());
	};

	env.initializeTest(testFunction);
	env.exec();
}

TEST(OptionsTests, FollowExternalNofollowLinkOnlyOption)
{
	TestEnvironment env;

	auto options = TestEnvironment::defaultOptions({ Url("http://options.com/index.html") });
	options.checkExternalLinks = true;
	options.followExternalNofollow = true;

	env.crawler()->options()->setData(options);

	const auto testFunction = [cl = env.crawler()]()
	{
		cl->waitForAllCrawledPageReceived(10);
		//cl->checkSequencedDataCollectionConsistency();

		std::vector<const ParsedPage*> crawledPages = cl->storageItems(StorageType::CrawledUrlStorageType);

		const Url externalLink("http://justanothersite2.com");

		const auto findIterator = std::find_if(crawledPages.begin(), crawledPages.end(),
			[&](const ParsedPage* page) { return page->url == externalLink; });

		EXPECT_EQ(true, findIterator != crawledPages.end());
	};

	env.initializeTest(testFunction);
	env.exec();
}

TEST(OptionsTests, FollowSubdomainLinkOnlyOption)
{
	TestEnvironment env;

	auto options = TestEnvironment::defaultOptions({ Url("http://options.com/index.html") });
	options.checkSubdomains = true;

	env.crawler()->options()->setData(options);

	const auto testFunction = [cl = env.crawler()]()
	{
		cl->waitForAllCrawledPageReceived(10);
		//cl->checkSequencedDataCollectionConsistency();

		std::vector<const ParsedPage*> crawledPages = cl->storageItems(StorageType::CrawledUrlStorageType);

		const Url externalLink("http://subdomain.options.com");

		const auto findIterator = std::find_if(crawledPages.begin(), crawledPages.end(),
			[&](const ParsedPage* page) { return page->url == externalLink; });

		EXPECT_EQ(true, findIterator != crawledPages.end());
	};

	env.initializeTest(testFunction);
	env.exec();
}

TEST(OptionsTests, FollowToAllExternalLinks)
{
	TestEnvironment env;

	auto options = TestEnvironment::defaultOptions({ Url("http://options.com/index.html") });
	options.checkExternalLinks = true;
	options.followExternalNofollow = true;
	options.checkSubdomains = true;

	env.crawler()->options()->setData(options);

	const auto testFunction = [cl = env.crawler()]()
	{
		cl->waitForAllCrawledPageReceived(10);
		//cl->checkSequencedDataCollectionConsistency();

		std::vector<const ParsedPage*> crawledPages = cl->storageItems(StorageType::CrawledUrlStorageType);

		const Url externalLink("http://justanothersite.com");
		const Url externalNofollowLink("http://justanothersite2.com");
		const Url externalSubdomainLink("http://subdomain.options.com");

		const auto externalLinkFindIterator = std::find_if(crawledPages.begin(), crawledPages.end(),
			[&](const ParsedPage* page) { return page->url == externalLink; });

		const auto externalNofollowLinkFindIterator = std::find_if(crawledPages.begin(), crawledPages.end(),
			[&](const ParsedPage* page) { return page->url == externalLink; });

		const auto externalSubdomainLinkFindIterator = std::find_if(crawledPages.begin(), crawledPages.end(),
			[&](const ParsedPage* page) { return page->url == externalLink; });

		EXPECT_EQ(true, externalLinkFindIterator != crawledPages.end());
		EXPECT_EQ(true, externalNofollowLinkFindIterator != crawledPages.end());
		EXPECT_EQ(true, externalSubdomainLinkFindIterator != crawledPages.end());
	};

	env.initializeTest(testFunction);
	env.exec();
}


}