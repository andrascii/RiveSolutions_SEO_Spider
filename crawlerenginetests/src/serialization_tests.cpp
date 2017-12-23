namespace CrawlerEngineTests
{

TEST(SerializationTests, PagesSerialization)
{
	CrawlerOptions options = TestEnvironment::defaultOptions({ CustomUrl("http://sitemap.com/page-1.html") });
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
	
}