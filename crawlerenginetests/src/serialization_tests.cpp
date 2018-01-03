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

		ParsedPage* firstPage = const_cast<ParsedPage*>(pages[0]);

		firstPage->redirectedUrl = Url("http://redirected-url.com");
		firstPage->canonicalUrl = Url("http://canonical-url.com");
		firstPage->title = QString("Title");
		firstPage->contentType = QString("text/html");
		firstPage->metaRefresh = QString("meta-refresh");
		firstPage->metaDescription = QString("meta-description");
		firstPage->metaKeywords = QString("meta-keywords");
		firstPage->serverResponse = QString("server-response");
		firstPage->firstH1 = QString("first-h1");
		firstPage->secondH1 = QString("second-h1");
		firstPage->firstH2 = QString("first-h2");
		firstPage->secondH2 = QString("second-h2");
		firstPage->statusCode = Common::StatusCode::Forbidden403;
		firstPage->metaRobotsFlags = { { UserAgentType::AnyBot, MetaRobotsFlags(MetaRobotsNoFollow) } };
		firstPage->responseDate = QDateTime::currentDateTimeUtc();
		firstPage->lastModifiedDate = QDateTime::currentDateTimeUtc();
		firstPage->pageSizeKilobytes = 42;
		firstPage->wordCount = 99;
		firstPage->pageHash = 9999999;
		firstPage->hasSeveralTitleTags = true;
		firstPage->hasSeveralMetaDescriptionTags = false;
		firstPage->hasSeveralMetaKeywordsTags = true;
		firstPage->hasSeveralH1Tags = false;
		firstPage->hasSeveralEqualH2Tags = true;
		firstPage->hasMetaRefreshTag = false;
		firstPage->hasFrames = true;
		firstPage->isThisExternalPage = true;
		firstPage->resourceType = ResourceType::ResourceHtml;
		firstPage->rawResponse = "<html>...ÀÁÂÃÄ</html>";
		firstPage->pageLevel = 3;


		ParsedPage etalon = *firstPage;
		
		cl->saveToFile(QString("pages.json"));
		cl->waitForSerializationDone(25);
		cl->clearReceivedData();
		cl->loadFromFile(QString("pages.json"));
		cl->waitForDeserializationDone(25);
		
		auto deserializedPages = cl->waitForParsedPageReceived(StorageType::CrawledUrlStorageType, 6, 10, "Waiting for 6 crawled pages");

		EXPECT_EQ(pages.size(), deserializedPages.size());
		const ParsedPage* newFirstPage = deserializedPages[0];

		//EXPECT_NE(firstPage, newFirstPage);

		EXPECT_EQ(true, etalon.url.compare(newFirstPage->url));
		EXPECT_EQ(true, etalon.redirectedUrl.compare(newFirstPage->redirectedUrl));
		EXPECT_EQ(true, etalon.canonicalUrl.compare(newFirstPage->canonicalUrl));
		EXPECT_EQ(etalon.title, newFirstPage->title);
		EXPECT_EQ(etalon.contentType, newFirstPage->contentType);
		EXPECT_EQ(etalon.metaRefresh, newFirstPage->metaRefresh);
		EXPECT_EQ(etalon.metaDescription, newFirstPage->metaDescription);
		EXPECT_EQ(etalon.metaKeywords, newFirstPage->metaKeywords);
		EXPECT_EQ(etalon.serverResponse, newFirstPage->serverResponse);
		EXPECT_EQ(etalon.firstH1, newFirstPage->firstH1);
		EXPECT_EQ(etalon.secondH1, newFirstPage->secondH1);
		EXPECT_EQ(etalon.firstH2, newFirstPage->firstH2);
		EXPECT_EQ(etalon.secondH2, newFirstPage->secondH2);
		EXPECT_EQ(etalon.statusCode, newFirstPage->statusCode);
		EXPECT_EQ(etalon.metaRobotsFlags, newFirstPage->metaRobotsFlags);
		EXPECT_EQ(etalon.responseDate, newFirstPage->responseDate);
		EXPECT_EQ(etalon.lastModifiedDate, newFirstPage->lastModifiedDate);
		EXPECT_EQ(etalon.pageSizeKilobytes, newFirstPage->pageSizeKilobytes);
		EXPECT_EQ(etalon.wordCount, newFirstPage->wordCount);
		EXPECT_EQ(etalon.pageHash, newFirstPage->pageHash);
		EXPECT_EQ(etalon.hasSeveralTitleTags, newFirstPage->hasSeveralTitleTags);
		EXPECT_EQ(etalon.hasSeveralMetaDescriptionTags, newFirstPage->hasSeveralMetaDescriptionTags);
		EXPECT_EQ(etalon.hasSeveralMetaKeywordsTags, newFirstPage->hasSeveralMetaKeywordsTags);
		EXPECT_EQ(etalon.hasSeveralH1Tags, newFirstPage->hasSeveralH1Tags);
		EXPECT_EQ(etalon.hasSeveralEqualH2Tags, newFirstPage->hasSeveralEqualH2Tags);
		EXPECT_EQ(etalon.hasMetaRefreshTag, newFirstPage->hasMetaRefreshTag);
		EXPECT_EQ(etalon.hasFrames, newFirstPage->hasFrames);
		EXPECT_EQ(etalon.isThisExternalPage, newFirstPage->isThisExternalPage);
		EXPECT_EQ(etalon.resourceType, newFirstPage->resourceType);
		EXPECT_EQ(etalon.rawResponse, newFirstPage->rawResponse);
		EXPECT_EQ(etalon.pageLevel, newFirstPage->pageLevel);
		EXPECT_EQ(etalon.storages, newFirstPage->storages);
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