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
		firstPage->isBlockedForIndexing = true;
		firstPage->isBlockedByMetaRobots = true;
		firstPage->resourceType = ResourceType::ResourceHtml;
		firstPage->rawResponse = "<html>...ÀÁÂÃÄ</html>";
		firstPage->pageLevel = 3;
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
		EXPECT_EQ(etalon.isBlockedForIndexing, newFirstPage->isBlockedForIndexing);
		EXPECT_EQ(etalon.isBlockedByMetaRobots, newFirstPage->isBlockedByMetaRobots);
		EXPECT_EQ(etalon.resourceType, newFirstPage->resourceType);
		EXPECT_EQ(etalon.rawResponse, newFirstPage->rawResponse);
		EXPECT_EQ(etalon.pageLevel, newFirstPage->pageLevel);
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
	options.followRobotsTxtRules = true;
	options.userAgentToFollow = UserAgentType::AnyBot;
	options.parserTypeFlags = ImagesResourcesParserType;
	options.pauseRangeFrom = 1;
	options.pauseRangeTo = 2;
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
		CrawlerOptionsData newOptions = crawler->options()->data();

		EXPECT_EQ(true, crawler->options()->startCrawlingPage().compare(newOptions.startCrawlingPage));
		EXPECT_EQ(crawler->options()->limitMaxUrlLength(), newOptions.limitMaxUrlLength);
		EXPECT_EQ(crawler->options()->limitSearchTotal(), newOptions.limitSearchTotal);
		EXPECT_EQ(crawler->options()->limitTimeout(), newOptions.limitTimeout);
		EXPECT_EQ(crawler->options()->maxRedirectsToFollow(), newOptions.maxRedirectsToFollow);
		EXPECT_EQ(crawler->options()->maxLinksCountOnPage(), newOptions.maxLinksCountOnPage);
		EXPECT_EQ(crawler->options()->minTitleLength(), newOptions.minTitleLength);
		EXPECT_EQ(crawler->options()->maxTitleLength(), newOptions.maxTitleLength);
		EXPECT_EQ(crawler->options()->maxDescriptionLength(), newOptions.maxDescriptionLength);
		EXPECT_EQ(crawler->options()->minDescriptionLength(), newOptions.minDescriptionLength);
		EXPECT_EQ(crawler->options()->maxH1LengthChars(), newOptions.maxH1LengthChars);
		EXPECT_EQ(crawler->options()->maxH2LengthChars(), newOptions.maxH2LengthChars);
		EXPECT_EQ(crawler->options()->maxImageAltTextChars(), newOptions.maxImageAltTextChars);
		EXPECT_EQ(crawler->options()->maxImageSizeKb(), newOptions.maxImageSizeKb);
		EXPECT_EQ(crawler->options()->maxPageSizeKb(), newOptions.maxPageSizeKb);
		EXPECT_EQ(crawler->options()->useProxy(), newOptions.useProxy);
		EXPECT_EQ(crawler->options()->proxyHostName(), newOptions.proxyHostName);
		EXPECT_EQ(crawler->options()->proxyPort(), newOptions.proxyPort);
		EXPECT_EQ(crawler->options()->proxyUser(), newOptions.proxyUser);
		EXPECT_EQ(crawler->options()->proxyPassword(), newOptions.proxyPassword);
		EXPECT_EQ(crawler->options()->checkExternalLinks(), newOptions.checkExternalLinks);
		EXPECT_EQ(crawler->options()->followInternalNofollow(), newOptions.followInternalNofollow);
		EXPECT_EQ(crawler->options()->followExternalNofollow(), newOptions.followExternalNofollow);
		EXPECT_EQ(crawler->options()->checkCanonicals(), newOptions.checkCanonicals);
		EXPECT_EQ(crawler->options()->checkSubdomains(), newOptions.checkSubdomains);
		EXPECT_EQ(crawler->options()->crawlOutsideOfStartFolder(), newOptions.crawlOutsideOfStartFolder);
		EXPECT_EQ(crawler->options()->followRobotsTxtRules(), newOptions.followRobotsTxtRules);
		EXPECT_EQ(crawler->options()->userAgentToFollow(), newOptions.userAgentToFollow);
		EXPECT_EQ(crawler->options()->parserTypeFlags(), newOptions.parserTypeFlags);
		EXPECT_EQ(crawler->options()->pauseRangeFrom(), newOptions.pauseRangeFrom);
		EXPECT_EQ(crawler->options()->pauseRangeTo(), newOptions.pauseRangeTo);
		EXPECT_EQ(crawler->options()->userAgent(), newOptions.userAgent);
		EXPECT_EQ(crawler->options()->searchYandexMetricaCounters(), newOptions.searchYandexMetricaCounters);
		EXPECT_EQ(crawler->options()->searchYandexMetricaCounter1(), newOptions.searchYandexMetricaCounter1);
		EXPECT_EQ(crawler->options()->yandexMetricaCounter1Id(), newOptions.yandexMetricaCounter1Id);
		EXPECT_EQ(crawler->options()->searchYandexMetricaCounter2(), newOptions.searchYandexMetricaCounter2);
		EXPECT_EQ(crawler->options()->yandexMetricaCounter2Id(), newOptions.yandexMetricaCounter2Id);
		EXPECT_EQ(crawler->options()->searchYandexMetricaCounter3(), newOptions.searchYandexMetricaCounter3);
		EXPECT_EQ(crawler->options()->yandexMetricaCounter3Id(), newOptions.yandexMetricaCounter3Id);
		EXPECT_EQ(crawler->options()->searchYandexMetricaCounter4(), newOptions.searchYandexMetricaCounter4);
		EXPECT_EQ(crawler->options()->yandexMetricaCounter4Id(), newOptions.yandexMetricaCounter4Id);
		EXPECT_EQ(crawler->options()->searchYandexMetricaCounter5(), newOptions.searchYandexMetricaCounter5);
		EXPECT_EQ(crawler->options()->yandexMetricaCounter5Id(), newOptions.yandexMetricaCounter5Id);
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