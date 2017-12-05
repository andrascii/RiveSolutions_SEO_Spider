namespace CrawlerEngineTests
{

TEST(SerializationTests, PagesSerialization)
{
	CrawlerOptions options = TestEnvironment::defaultOptions({ QUrl("http://sitemap.com/page-1.html") });
	options.parserTypeFlags = ImagesResourcesParserType;
	TestEnvironment env(options);

	const auto testFunction = [cl = env.crawler()]()
	{
		auto pages = cl->waitForParsedPageReceived(CrawledUrlStorageType, 6, 10, "Waiting for 6 crawled pages");
		cl->checkSequencedDataCollectionConsistency();

		cl->saveToFile(QString("pages.json"));
		cl->stopCrawling();
		cl->loadFromFile(QString("pages.json"));
		
		auto deserializedPages = cl->waitForParsedPageReceived(CrawledUrlStorageType, 6, 10, "Waiting for 6 crawled pages");
		EXPECT_EQ(pages.size(), deserializedPages.size());
		for (size_t i = 0; i < pages.size(); ++i)
		{
			const ParsedPage* page = pages[i];
			const ParsedPage* deserializedPage = deserializedPages[i];

			// Cannot check, pages are destroyed at this moment
			//EXPECT_EQ(page->linksOnThisPage.size(), deserializedPage->linksOnThisPage.size());
			//EXPECT_EQ(page->linksToThisPage.size(), deserializedPage->linksToThisPage.size());

		}
		// TODO: compare pages and deserializedPages
	};

	env.initializeTest(testFunction);
	env.exec();
}
	
}