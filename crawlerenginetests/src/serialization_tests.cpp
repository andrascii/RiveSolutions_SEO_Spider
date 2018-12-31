namespace CrawlerEngineTests
{

TEST(SerializationTests, PagesSerialization)
{
	TestEnvironment env;

	auto options = TestEnvironment::defaultOptions({ Url("http://sitemap.com/page-1.html") });
	options.parserTypeFlags = ImagesResourcesParserType;
	options.searchYandexMetricaCounters = true;
	options.searchYandexMetricaCounter1 = true;
	options.yandexMetricaCounter1Id = 1;
	options.searchYandexMetricaCounter2 = true;
	options.yandexMetricaCounter2Id = 2;
	options.searchYandexMetricaCounter3 = true;
	options.yandexMetricaCounter3Id = 3;
	options.searchYandexMetricaCounter4 = true;
	options.yandexMetricaCounter4Id = 4;
	options.searchYandexMetricaCounter5 = true;
	options.yandexMetricaCounter5Id = 5;

	env.crawler()->options()->setData(options);

	const auto testFunction = [cl = env.crawler()]()
	{
		auto pages = cl->waitForParsedPageReceived(StorageType::CrawledUrlStorageType, 6, 10, "Waiting for 6 crawled pages");
		cl->waitForCrawlingDone();
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
		firstPage->secondsToRefresh = 10;
		firstPage->pageHash = 9999999;
		firstPage->hasSeveralTitleTags = true;
		firstPage->hasSeveralMetaDescriptionTags = false;
		firstPage->hasSeveralMetaKeywordsTags = true;
		firstPage->hasSeveralH1Tags = false;
		firstPage->hasSeveralEqualH2Tags = true;
		firstPage->hasMetaRefreshTag = false;
		firstPage->hasFrames = true;
		firstPage->isThisExternalPage = true;
		firstPage->isBlockedByMetaRobots = true;
		firstPage->resourceType = ResourceType::ResourceHtml;
		firstPage->rawResponse = "<html>...ÀÁÂÃÄ</html>";
		firstPage->pageLevel = 3;
		firstPage->responseTime = 100;
		firstPage->missingYandexMetricaCounters = std::vector<StorageType>
		{
			YandexMetricaCounter1StorageType,
			YandexMetricaCounter2StorageType,
			YandexMetricaCounter3StorageType,
			YandexMetricaCounter4StorageType,
			YandexMetricaCounter5StorageType
		};

		ParsedPage etalon = *firstPage;

		cl->saveToFileSafe(QString("pages.json"));
		cl->waitForSerializationDone(25);
		cl->clearReceivedData();
		cl->loadFromFileSafe(QString("pages.json"));
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
		EXPECT_EQ(etalon.secondsToRefresh, newFirstPage->secondsToRefresh);
		EXPECT_EQ(etalon.pageHash, newFirstPage->pageHash);
		EXPECT_EQ(etalon.hasSeveralTitleTags, newFirstPage->hasSeveralTitleTags);
		EXPECT_EQ(etalon.hasSeveralMetaDescriptionTags, newFirstPage->hasSeveralMetaDescriptionTags);
		EXPECT_EQ(etalon.hasSeveralMetaKeywordsTags, newFirstPage->hasSeveralMetaKeywordsTags);
		EXPECT_EQ(etalon.hasSeveralH1Tags, newFirstPage->hasSeveralH1Tags);
		EXPECT_EQ(etalon.hasSeveralEqualH2Tags, newFirstPage->hasSeveralEqualH2Tags);
		EXPECT_EQ(etalon.hasMetaRefreshTag, newFirstPage->hasMetaRefreshTag);
		EXPECT_EQ(etalon.hasFrames, newFirstPage->hasFrames);
		EXPECT_EQ(etalon.isThisExternalPage, newFirstPage->isThisExternalPage);
		EXPECT_EQ(etalon.isBlockedByMetaRobots, newFirstPage->isBlockedByMetaRobots);
		EXPECT_EQ(etalon.resourceType, newFirstPage->resourceType);
		EXPECT_EQ(etalon.rawResponse, newFirstPage->rawResponse);
		EXPECT_EQ(etalon.pageLevel, newFirstPage->pageLevel);
		EXPECT_EQ(etalon.responseTime, newFirstPage->responseTime);
		EXPECT_EQ(etalon.storages, newFirstPage->storages);
		EXPECT_EQ(etalon.missingYandexMetricaCounters, newFirstPage->missingYandexMetricaCounters);
	};

	env.initializeTest(testFunction);
	env.exec();
}

TEST(SerializationTests, OptionsSerialization)
{
	TestEnvironment env;

	auto options = TestEnvironment::defaultOptions({ Url("http://sitemap.com/page-1.html") });
	options.limitMaxUrlLength = 150;
	options.limitSearchTotal = 10;
	options.limitTimeout = 10;
	options.maxRedirectsToFollow = 10;
	options.maxParallelConnections = 15;
	options.maxLinksCountOnPage = 10;
	options.minTitleLength = 11;
	options.maxTitleLength = 12;
	options.maxDescriptionLength = 14;
	options.minDescriptionLength = 13;
	options.maxH1LengthChars = 14;
	options.maxH2LengthChars = 15;
	options.maxImageAltTextChars = 16;
	options.maxImageSizeKb = 17;
	options.maxPageSizeKb = 18;
	options.useProxy = true;
	options.proxyHostName = QString("proxy host name");
	options.proxyPort = 8080;
	options.proxyUser = QString("proxy user");
	options.proxyPassword = QString("proxy password");
	options.checkExternalLinks = true;
	options.followInternalNofollow = true;
	options.followExternalNofollow = true;
	options.checkCanonicals = true;
	options.checkSubdomains = true;
	options.crawlOutsideOfStartFolder = true;
	options.crawlMetaHrefLangLinks = true;
	options.followRobotsTxtRules = true;
	options.userAgentToFollow = UserAgentType::AnyBot;
	options.parserTypeFlags = ImagesResourcesParserType;
	options.pauseRangeFrom = 1;
	options.pauseRangeTo = 2;
	options.pauseRangeEnabled = true;
	options.userAgent = "BOT";
	options.searchYandexMetricaCounters = true;
	options.searchYandexMetricaCounter1 = true;
	options.yandexMetricaCounter1Id = 1;
	options.searchYandexMetricaCounter2 = true;
	options.yandexMetricaCounter2Id = 2;
	options.searchYandexMetricaCounter3 = true;
	options.yandexMetricaCounter3Id = 3;
	options.searchYandexMetricaCounter4 = true;
	options.yandexMetricaCounter4Id = 4;
	options.searchYandexMetricaCounter5 = true;
	options.yandexMetricaCounter5Id = 5;

	env.crawler()->options()->setData(options);

	const auto testFunction = [crawler = env.crawler(), &options]()
	{
		crawler->waitForCrawlingDone();
		auto pages = crawler->storageItems(StorageType::CrawledUrlStorageType);

		EXPECT_EQ(pages.size(), 5);

		crawler->checkSequencedDataCollectionConsistency();
		crawler->saveToFileSafe(QString("options.json"));

		crawler->waitForSerializationDone(10);

		EXPECT_EQ(crawler->state(), Crawler::StatePending);

		crawler->clearData();
		crawler->waitForClearingDataDone(5);
		crawler->options()->setData(TestEnvironment::defaultOptions({ Url("http://sitemap.com/page-5.html") }));

		// test case and crawler works in a different threads
		VERIFY(QMetaObject::invokeMethod(crawler, "startCrawling", Qt::BlockingQueuedConnection));

		crawler->waitForAllCrawledPageReceived(10);
		crawler->stopCrawling();

		crawler->loadFromFileSafe(QString("options.json"));
		crawler->waitForDeserializationDone(10);

		EXPECT_EQ(true, crawler->options()->startCrawlingPage().compare(options.startCrawlingPage));
		EXPECT_EQ(crawler->options()->limitMaxUrlLength(), options.limitMaxUrlLength);
		EXPECT_EQ(crawler->options()->limitSearchTotal(), options.limitSearchTotal);
		EXPECT_EQ(crawler->options()->limitTimeout(), options.limitTimeout);
		EXPECT_EQ(crawler->options()->maxRedirectsToFollow(), options.maxRedirectsToFollow);
		EXPECT_EQ(crawler->options()->maxParallelConnections(), options.maxParallelConnections);
		EXPECT_EQ(crawler->options()->maxLinksCountOnPage(), options.maxLinksCountOnPage);
		EXPECT_EQ(crawler->options()->minTitleLength(), options.minTitleLength);
		EXPECT_EQ(crawler->options()->maxTitleLength(), options.maxTitleLength);
		EXPECT_EQ(crawler->options()->maxDescriptionLength(), options.maxDescriptionLength);
		EXPECT_EQ(crawler->options()->minDescriptionLength(), options.minDescriptionLength);
		EXPECT_EQ(crawler->options()->maxH1LengthChars(), options.maxH1LengthChars);
		EXPECT_EQ(crawler->options()->maxH2LengthChars(), options.maxH2LengthChars);
		EXPECT_EQ(crawler->options()->maxImageAltTextChars(), options.maxImageAltTextChars);
		EXPECT_EQ(crawler->options()->maxImageSizeKb(), options.maxImageSizeKb);
		EXPECT_EQ(crawler->options()->maxPageSizeKb(), options.maxPageSizeKb);
		EXPECT_EQ(crawler->options()->useProxy(), options.useProxy);
		EXPECT_EQ(crawler->options()->proxyHostName(), options.proxyHostName);
		EXPECT_EQ(crawler->options()->proxyPort(), options.proxyPort);
		EXPECT_EQ(crawler->options()->proxyUser(), options.proxyUser);
		EXPECT_EQ(crawler->options()->proxyPassword(), options.proxyPassword);
		EXPECT_EQ(crawler->options()->checkExternalLinks(), options.checkExternalLinks);
		EXPECT_EQ(crawler->options()->followInternalNofollow(), options.followInternalNofollow);
		EXPECT_EQ(crawler->options()->followExternalNofollow(), options.followExternalNofollow);
		EXPECT_EQ(crawler->options()->checkCanonicals(), options.checkCanonicals);
		EXPECT_EQ(crawler->options()->checkSubdomains(), options.checkSubdomains);
		EXPECT_EQ(crawler->options()->crawlOutsideOfStartFolder(), options.crawlOutsideOfStartFolder);
		EXPECT_EQ(crawler->options()->crawlMetaHrefLangLinks(), options.crawlMetaHrefLangLinks);
		EXPECT_EQ(crawler->options()->followRobotsTxtRules(), options.followRobotsTxtRules);
		EXPECT_EQ(crawler->options()->userAgentToFollow(), options.userAgentToFollow);
		EXPECT_EQ(crawler->options()->parserTypeFlags(), options.parserTypeFlags);
		EXPECT_EQ(crawler->options()->pauseRangeFrom(), options.pauseRangeFrom);
		EXPECT_EQ(crawler->options()->pauseRangeTo(), options.pauseRangeTo);
		EXPECT_EQ(crawler->options()->pauseRangeEnabled(), options.pauseRangeEnabled);
		EXPECT_EQ(crawler->options()->userAgent(), options.userAgent);
		EXPECT_EQ(crawler->options()->searchYandexMetricaCounters(), options.searchYandexMetricaCounters);
		EXPECT_EQ(crawler->options()->searchYandexMetricaCounter1(), options.searchYandexMetricaCounter1);
		EXPECT_EQ(crawler->options()->yandexMetricaCounter1Id(), options.yandexMetricaCounter1Id);
		EXPECT_EQ(crawler->options()->searchYandexMetricaCounter2(), options.searchYandexMetricaCounter2);
		EXPECT_EQ(crawler->options()->yandexMetricaCounter2Id(), options.yandexMetricaCounter2Id);
		EXPECT_EQ(crawler->options()->searchYandexMetricaCounter3(), options.searchYandexMetricaCounter3);
		EXPECT_EQ(crawler->options()->yandexMetricaCounter3Id(), options.yandexMetricaCounter3Id);
		EXPECT_EQ(crawler->options()->searchYandexMetricaCounter4(), options.searchYandexMetricaCounter4);
		EXPECT_EQ(crawler->options()->yandexMetricaCounter4Id(), options.yandexMetricaCounter4Id);
		EXPECT_EQ(crawler->options()->searchYandexMetricaCounter5(), options.searchYandexMetricaCounter5);
		EXPECT_EQ(crawler->options()->yandexMetricaCounter5Id(), options.yandexMetricaCounter5Id);
	};

	env.initializeTest(testFunction);
	env.exec();
}

TEST(SerializationTests, HostInfoSerialization)
{
	TestEnvironment env;
	env.crawler()->options()->setData(TestEnvironment::defaultOptions({ Url("http://sitemap.com/page-1.html") }));

	const auto testFunction = [crawler = env.crawler()]()
	{
		auto pages = crawler->waitForAllCrawledPageReceived(5);

		crawler->checkSequencedDataCollectionConsistency();

		auto data = crawler->webHostInfo()->allData();

		crawler->saveToFileSafe(QString("data.json"));
		crawler->waitForSerializationDone(5);
		crawler->clearData();
		crawler->waitForClearingDataDone(5);
		crawler->options()->setData(TestEnvironment::defaultOptions({ Url("http://sitemap.com/page-5.html") }));

		// test case and crawler works in a different threads
		VERIFY(QMetaObject::invokeMethod(crawler, "startCrawling", Qt::BlockingQueuedConnection));

		crawler->waitForAllCrawledPageReceived(10);

		CrawlerOptionsData newOptions;

		crawler->loadFromFileSafe(QString("data.json"));
		crawler->waitForDeserializationDone(10);

		auto deserializedData = crawler->webHostInfo()->allData();

		const auto cannonizeLineBreaks = [](QByteArray& value)
		{
			return value.replace('\r', QByteArray(""));
		};

		EXPECT_EQ(data.image, deserializedData.image);
		EXPECT_EQ(data.robotstxtUrl, deserializedData.robotstxtUrl);
		EXPECT_EQ(data.isRobotstxtValid, deserializedData.isRobotstxtValid);
		EXPECT_EQ(cannonizeLineBreaks(data.robotstxtContent), cannonizeLineBreaks(deserializedData.robotstxtContent));
		EXPECT_EQ(data.siteMapUrl, deserializedData.siteMapUrl);
		EXPECT_EQ(data.isSiteMapValid, deserializedData.isSiteMapValid);
		EXPECT_EQ(cannonizeLineBreaks(data.siteMapContent), cannonizeLineBreaks(deserializedData.siteMapContent));
	};

	env.initializeTest(testFunction);
	env.exec();
}

}