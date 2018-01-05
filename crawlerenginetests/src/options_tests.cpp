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

TEST(OptionsTests, NofollowLinksMustNotBeLoaded)
{
	CrawlerOptions options = TestEnvironment::defaultOptions(Url("http://nofollowlinks.com"));
	options.followInternalNofollow = false;

	TestEnvironment env(options);

	const auto testFunction = [cl = env.crawler()]()
	{
		auto pages = cl->waitForAllCrawledPageReceived(10);
		cl->checkSequencedDataCollectionConsistency();

		std::vector<const ParsedPage*> crawledPages = cl->storageItems(StorageType::CrawledUrlStorageType);
		std::vector<const ParsedPage*> pendingPages = cl->getLinksFromUnorderedDataCollection(StorageType::PendingResourcesStorageType);

		std::vector<Url> nofollowLinks;

		const auto addNofollowLink = [&nofollowLinks](const auto& resourceLink)
		{
			if (resourceLink.resource.expired() || resourceLink.linkParameter != LinkParameter::NofollowParameter)
			{
				return;
			}

			nofollowLinks.push_back(resourceLink.resource.lock().get()->url);
		};

		const auto collectNofollowLinks = [&addNofollowLink](auto* page)
		{
			std::for_each(page->linksOnThisPage.begin(), page->linksOnThisPage.end(), addNofollowLink);
		};

		std::for_each(crawledPages.begin(), crawledPages.end(), collectNofollowLinks);

		const auto checkLinks = [&nofollowLinks](auto* page)
		{
			for (const Url& nofollowLink : nofollowLinks)
			{
				EXPECT_NE(nofollowLink, page->url);
			}
		};

		std::for_each(pendingPages.begin(), pendingPages.end(), checkLinks);
		std::for_each(crawledPages.begin(), crawledPages.end(), checkLinks);
	};

	env.initializeTest(testFunction);
	env.exec();
}

TEST(OptionsTests, SubdomainsMustNotBeLoaded)
{
	const Url baseUrl("http://subdomains.com");
	CrawlerOptions options = TestEnvironment::defaultOptions(baseUrl);

	TestEnvironment env(options);

	const auto testFunction = [cl = env.crawler(), &baseUrl]()
	{
		auto pages = cl->waitForAllCrawledPageReceived(10);
		cl->checkSequencedDataCollectionConsistency();

		std::vector<const ParsedPage*> crawledPages = cl->storageItems(StorageType::CrawledUrlStorageType);
		std::vector<const ParsedPage*> pendingPages = cl->getLinksFromUnorderedDataCollection(StorageType::PendingResourcesStorageType);

		const auto check = [&](const ParsedPage* page)
		{
			EXPECT_EQ(false, PageParserHelpers::isSubdomain(baseUrl, page->url));
		};

		std::for_each(crawledPages.begin(), crawledPages.end(), check);
		std::for_each(pendingPages.begin(), pendingPages.end(), check);
	};

	env.initializeTest(testFunction);
	env.exec();
}

TEST(OptionsTests, BlockedByRobotsTxtLinksMustNotBeLoaded)
{
	CrawlerOptions options = TestEnvironment::defaultOptions(Url("http://blockedbyrobotstxt.com"));
	options.followRobotsTxtRules = true;
	options.userAgentToFollow = UserAgentType::AnyBot;

	TestEnvironment env(options);

	const auto testFunction = [cl = env.crawler()]()
	{
		auto pages = cl->waitForAllCrawledPageReceived(10);
		cl->checkSequencedDataCollectionConsistency();

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
	CrawlerOptions options = TestEnvironment::defaultOptions({ Url("http://options.com/index.html") });
	options.checkExternalLinks = true;

	TestEnvironment env(options);

	const auto testFunction = [cl = env.crawler()]()
	{
		cl->waitForAllCrawledPageReceived(10);
		cl->checkSequencedDataCollectionConsistency();

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
	CrawlerOptions options = TestEnvironment::defaultOptions({ Url("http://options.com/index.html") });
	options.followExternalNofollow = true;

	TestEnvironment env(options);

	const auto testFunction = [cl = env.crawler()]()
	{
		cl->waitForAllCrawledPageReceived(10);
		cl->checkSequencedDataCollectionConsistency();

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
	CrawlerOptions options = TestEnvironment::defaultOptions({ Url("http://options.com/index.html") });
	options.checkSubdomains = true;

	TestEnvironment env(options);

	const auto testFunction = [cl = env.crawler()]()
	{
		cl->waitForAllCrawledPageReceived(10);
		cl->checkSequencedDataCollectionConsistency();

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
	CrawlerOptions options = TestEnvironment::defaultOptions({ Url("http://options.com/index.html") });
	options.checkExternalLinks = true;
	options.followExternalNofollow = true;
	options.checkSubdomains = true;

	TestEnvironment env(options);

	const auto testFunction = [cl = env.crawler()]()
	{
		cl->waitForAllCrawledPageReceived(10);
		cl->checkSequencedDataCollectionConsistency();

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