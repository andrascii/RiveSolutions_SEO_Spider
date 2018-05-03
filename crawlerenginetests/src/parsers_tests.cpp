namespace CrawlerEngineTests
{

TEST(ParsersTests, LinksParserFromHrefAndSrc)
{
	// checks validness of parsing links from tags <a></a> and <img>

	const Url url("http://links-parser.com/index.html");

	TestEnvironment env;
	env.crawler()->options()->setData(TestEnvironment::defaultOptions(url));

	const auto testFunction = [cl = env.crawler(), &url]()
	{
		cl->waitForCrawlingDone();

		const std::vector<Url> urls =
		{
			Url("http://links-parser.com/image_with_break_line.png"),
			Url("http://links-parser.com/image_with_tabs.png"),
			Url("http://links-parser.com/link_with_break_line.php"),
			Url("http://links-parser.com/link_with_tabs.php")
		};

		std::vector<const ParsedPage*> crawledLinks = cl->storageItems(StorageType::CrawledUrlStorageType);
		const auto iter = std::find_if(crawledLinks.begin(), crawledLinks.end(), [&](const ParsedPage* parsedPage) { return parsedPage->url.compare(url); });

		EXPECT_EQ(false, iter == crawledLinks.end());

		if (iter != crawledLinks.end())
		{
			for (const ResourceLink& link : (*iter)->linksOnThisPage)
			{
				const auto correctedUrlIter = std::find(urls.begin(), urls.end(), link.url);

				EXPECT_EQ(true, correctedUrlIter != urls.end());
			}
		}
	};

	env.initializeTest(testFunction);
	env.exec();
}

}