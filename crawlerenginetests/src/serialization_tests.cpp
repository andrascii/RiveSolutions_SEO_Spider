namespace CrawlerEngineTests
{

TEST(SerializationTests, PagesSerialization)
{
	CrawlerEngine::CrawlerOptions options = TestEnvironment::defaultOptions({ QUrl("http://sitemap.com/page-1.html") });
	options.parserTypeFlags = CrawlerEngine::ImagesResourcesParserType;
	TestEnvironment env(options);

	const auto testFunction = [cl = env.crawler()]()
	{
		cl->waitForParsedPageReceived(CrawlerEngine::StorageType::CrawledUrlStorageType, 6, 10, "Waiting for 6 crawled pages");
		cl->checkSequencedDataCollectionConsistency();

		cl->saveToFile(QString("pages.json"));

	};

	env.initializeTest(testFunction);
	env.exec();
}
	
}