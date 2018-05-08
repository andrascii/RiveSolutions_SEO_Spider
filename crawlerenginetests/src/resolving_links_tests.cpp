namespace CrawlerEngineTests
{

TEST(ResolvingLinksTests, BaseTagTest)
{
	TestEnvironment env;
	env.crawler()->options()->setData(TestEnvironment::defaultOptions(Url("http://resolving-links.com/index.html")));

	const auto testFunction = [cl = env.crawler()]()
	{
		cl->waitForCrawlingDone();
		cl->checkSequencedDataCollectionConsistency();

		const std::vector<Url> etalonLinks
		{
			Url("http://resolving-links.com/index.html"),
			Url("http://resolving-links.com/link_to_this_page_from_folder.html"),
			Url("http://resolving-links.com/canonical_link.html"),
			Url("http://resolving-links.com/folder/page.html"),
			Url("http://resolving-links.com/folder/canonical_link.html")
		};

		const std::vector<const ParsedPage*> crawledPages = cl->storageItems(StorageType::CrawledUrlStorageType);

		EXPECT_EQ(etalonLinks.size(), crawledPages.size());

		for (size_t i = 0; i < etalonLinks.size(); ++i)
		{
			const auto findLambda = [etalonLink = etalonLinks[i]](const ParsedPage* page)
			{
				return etalonLink.canonizedUrlStr() == page->url.canonizedUrlStr();
			};

			const auto iter = std::find_if(crawledPages.begin(), crawledPages.end(), findLambda);

			EXPECT_EQ(true, iter != crawledPages.end());
		}
	};

	env.initializeTest(testFunction);
	env.exec();
}

}