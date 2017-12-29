namespace CrawlerEngineTests
{

TEST(SerializationTests, PagesSerialization)
{
	CrawlerOptions options = TestEnvironment::defaultOptions({ Url("http://sitemap.com/page-1.html") });
	options.parserTypeFlags = ImagesResourcesParserType;
	TestEnvironment env(options);

	const auto testFunction = [cl = env.crawler()]()
	{
		auto pages = cl->waitForParsedPageReceived(StorageType::CrawledUrlStorageType, 6, 10, "Waiting for 6 crawled pages");
		cl->checkSequencedDataCollectionConsistency();

		cl->stopCrawling();
		cl->saveToFile(QString("pages.json"));
		cl->loadFromFile(QString("pages.json"));
		
		auto deserializedPages = cl->waitForParsedPageReceived(StorageType::CrawledUrlStorageType, 6, 10, "Waiting for 6 crawled pages");

		EXPECT_EQ(pages.size(), deserializedPages.size());

		for (size_t i = 0; i < pages.size(); ++i)
		{
			const ParsedPage* page = pages[i];
			const ParsedPage* deserializedPage = deserializedPages[i];
		}
	};

	env.initializeTest(testFunction);
	env.exec();
}

TEST(SerializationTests, OptionsSerialization)
{
	CrawlerOptions options = TestEnvironment::defaultOptions({ Url("http://sitemap.com/page-1.html") });
	options.limitMaxUrlLength = 10;
	options.minTitleLength = 11;
	options.maxTitleLength = 12;
	options.maxDescriptionLength = 14;
	options.minDescriptionLength = 13;
	options.maxH1LengthChars = 14;
	options.maxH2LengthChars = 15;
	options.maxImageAltTextChars = 16;
	options.maxImageSizeKb = 17;
	options.checkExternalLinks = true;
	options.followInternalNofollow = true;
	options.followExternalNofollow = true;
	options.checkCanonicals = true;
	options.checkSubdomains = true;
	options.crawlOutsideOfStartFolder = true;
	options.followRobotsTxtRules = true;
	options.userAgentToFollow = UserAgentType::AnyBot;
	options.parserTypeFlags = ImagesResourcesParserType;
	options.pauseRangeFrom = 1;
	options.pauseRangeTo = 2;
	options.userAgent = "BOT";

	TestEnvironment env(options);

	const auto testFunction = [crawler = env.crawler(), &options]()
	{
		auto pages = crawler->waitForParsedPageReceived(StorageType::CrawledUrlStorageType, 6, 10, "Waiting for 6 crawled pages");

		crawler->checkSequencedDataCollectionConsistency();
		crawler->stopCrawling();
		crawler->saveToFile(QString("options.json"));

		const auto afterSavingData = [crawler, &options]
		{
			crawler->clearData();
			crawler->startCrawling(TestEnvironment::defaultOptions({ Url("http://sitemap.com/page-5.html") }));
			crawler->waitForAllCrawledPageReceived(10);

			CrawlerOptions newOptions;

			VERIFY(QObject::connect(crawler, &Crawler::crawlerOptionsChanged, [&newOptions](CrawlerOptions opts)
			{
				newOptions = opts;
			}));

			crawler->loadFromFile(QString("options.json"));
			crawler->waitForParsedPageReceived(StorageType::CrawledUrlStorageType, 6, 10, "Waiting for 6 crawled pages");

			EXPECT_EQ(true, options.host.compare(newOptions.host));
			EXPECT_EQ(options.limitMaxUrlLength, newOptions.limitMaxUrlLength);
			EXPECT_EQ(options.minTitleLength, newOptions.minTitleLength);
			EXPECT_EQ(options.maxTitleLength, newOptions.maxTitleLength);
			EXPECT_EQ(options.maxDescriptionLength, newOptions.maxDescriptionLength);
			EXPECT_EQ(options.minDescriptionLength, newOptions.minDescriptionLength);
			EXPECT_EQ(options.maxH1LengthChars, newOptions.maxH1LengthChars);
			EXPECT_EQ(options.maxH2LengthChars, newOptions.maxH2LengthChars);
			EXPECT_EQ(options.maxImageAltTextChars, newOptions.maxImageAltTextChars);
			EXPECT_EQ(options.maxImageSizeKb, newOptions.maxImageSizeKb);
			EXPECT_EQ(options.checkExternalLinks, newOptions.checkExternalLinks);
			EXPECT_EQ(options.followInternalNofollow, newOptions.followInternalNofollow);
			EXPECT_EQ(options.followExternalNofollow, newOptions.followExternalNofollow);
			EXPECT_EQ(options.checkCanonicals, newOptions.checkCanonicals);
			EXPECT_EQ(options.checkSubdomains, newOptions.checkSubdomains);
			EXPECT_EQ(options.crawlOutsideOfStartFolder, newOptions.crawlOutsideOfStartFolder);
			EXPECT_EQ(options.followRobotsTxtRules, newOptions.followRobotsTxtRules);
			EXPECT_EQ(options.userAgentToFollow, newOptions.userAgentToFollow);
			EXPECT_EQ(options.parserTypeFlags, newOptions.parserTypeFlags);
			EXPECT_EQ(options.pauseRangeFrom, newOptions.pauseRangeFrom);
			EXPECT_EQ(options.pauseRangeTo, newOptions.pauseRangeTo);
			EXPECT_EQ(options.userAgent, newOptions.userAgent);
		};

		VERIFY(QObject::connect(crawler, &Crawler::serializationProcessDone, afterSavingData));
	};

	env.initializeTest(testFunction);
	env.exec();
}
	
}