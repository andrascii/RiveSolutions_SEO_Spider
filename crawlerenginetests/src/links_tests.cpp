namespace CrawlerEngineTests
{
	
TEST(LinksTests, LinkAlt)
{
	TestEnvironment env(TestEnvironment::defaultOptions(Url("http://links.com/link-with-title.html")));

	const auto testFunction = [cl = env.crawler()]()
	{
		auto pages = cl->waitForParsedPageReceived(StorageType::CrawledUrlStorageType, 2, 10, "Waiting for 2 crawled pages");
		cl->checkSequencedDataCollectionConsistency();
		EXPECT_EQ(2, pages.size());
		EXPECT_EQ(0, pages[0]->linksToThisPage.size());
		EXPECT_EQ(1, pages[0]->linksOnThisPage.size());
		EXPECT_EQ(Common::StatusCode::Ok200, pages[0]->statusCode);

		QString expectedTitle = QString::fromWCharArray(L"This is a broken link и чето по русски");
		EXPECT_EQ(expectedTitle, pages[0]->linksOnThisPage[0].altOrTitle);
	};

	env.initializeTest(testFunction);
	env.exec();
}

TEST(LinksTests, CanonicalNextPrev)
{
	TestEnvironment env(TestEnvironment::defaultOptions(Url("http://links.com/canonical-next-prev.html")));

	const auto testFunction = [cl = env.crawler()]()
	{
		auto pages = cl->waitForParsedPageReceived(StorageType::CrawledUrlStorageType, 4, 10, "Waiting for 4 crawled pages");
		cl->checkSequencedDataCollectionConsistency();
		EXPECT_EQ(4, pages.size());

		auto canonical = pages[0]->linksOnThisPage[0];
		auto next = pages[0]->linksOnThisPage[1];
		auto prev = pages[0]->linksOnThisPage[2];

		EXPECT_EQ(QString("http://links.com/next-prev-canonical.html"), canonical.resource.lock()->url.toDisplayString());
		EXPECT_EQ(QString("http://links.com/next.html"), next.resource.lock()->url.toDisplayString());
		EXPECT_EQ(QString("http://links.com/prev.html"), prev.resource.lock()->url.toDisplayString());

		EXPECT_EQ(ResourceSource::SourceTagLinkRelCanonical, canonical.resourceSource);
		EXPECT_EQ(ResourceSource::SourceTagLinkRelNext, next.resourceSource);
		EXPECT_EQ(ResourceSource::SourceTagLinkRelPrev, prev.resourceSource);
	};

	env.initializeTest(testFunction);
	env.exec();
}

TEST(LinksTests, NofollowLinksMustNotBeLoaded)
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

		std::vector<QUrl> nofollowLinks;

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
			for (const QUrl& nofollowLink : nofollowLinks)
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

TEST(LinksTests, SubdomainsMustNotBeLoaded)
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

		std::vector<QUrl> subdomainLinks;

		const auto addSubdomainLink = [&subdomainLinks, &baseUrl](const auto& resourceLink)
		{
			if (!PageParserHelpers::isSubdomain(baseUrl, resourceLink.url))
			{
				return;
			}

			subdomainLinks.push_back(resourceLink.url);
		};

		const auto collectSubdomainLinks = [&addSubdomainLink](auto* page)
		{
			std::for_each(page->linksOnThisPage.begin(), page->linksOnThisPage.end(), addSubdomainLink);
		};

		std::for_each(crawledPages.begin(), crawledPages.end(), collectSubdomainLinks);

		EXPECT_EQ(true, !subdomainLinks.empty());

		const auto checkLinks = [&subdomainLinks](auto* page)
		{
			for (const QUrl& subdomainLink : subdomainLinks)
			{
				EXPECT_NE(subdomainLink, page->url);
			}
		};

		std::for_each(pendingPages.begin(), pendingPages.end(), checkLinks);
		std::for_each(crawledPages.begin(), crawledPages.end(), checkLinks);
	};

	env.initializeTest(testFunction);
	env.exec();
}

TEST(LinksTests, BlockedByOutsideFolderLinksMustNotBeLoaded)
{
	CrawlerOptions options = TestEnvironment::defaultOptions(Url("http://links.com/folder1/folder2/index.html"));
	options.crawlOutsideOfStartFolder = false;

	TestEnvironment env(options);

	const auto testFunction = [cl = env.crawler()]()
	{
		auto pages = cl->waitForAllCrawledPageReceived(10);
		cl->checkSequencedDataCollectionConsistency();

		EXPECT_EQ(pages.size(), 2);

		std::vector<const ParsedPage*> crawledPages = cl->storageItems(StorageType::CrawledUrlStorageType);
		std::vector<const ParsedPage*> pendingPages = cl->getLinksFromUnorderedDataCollection(StorageType::PendingResourcesStorageType);

		const QUrl blockedByOutsideFolderUrl("http://links.com/folder1/index.html");

		const auto checkLinks = [&blockedByOutsideFolderUrl](auto* page)
		{
			EXPECT_NE(blockedByOutsideFolderUrl, page->url);
		};

		std::for_each(pendingPages.begin(), pendingPages.end(), checkLinks);
		std::for_each(crawledPages.begin(), crawledPages.end(), checkLinks);
	};

	env.initializeTest(testFunction);
	env.exec();
}

TEST(LinksTests, BlockedByRobotsTxtLinksMustNotBeLoaded)
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

		const QUrl blockedByRobotsUrl("http://blockedbyrobotstxt.com/blocked.html");

		const auto checkLinks = [&blockedByRobotsUrl](auto* page)
		{
			EXPECT_NE(blockedByRobotsUrl, page->url);
		};

		std::for_each(pendingPages.begin(), pendingPages.end(), checkLinks);
		std::for_each(crawledPages.begin(), crawledPages.end(), checkLinks);
	};

	env.initializeTest(testFunction);
	env.exec();
}

TEST(LinksTests, Canonical)
{
	TestEnvironment env(TestEnvironment::defaultOptions(Url("http://links.com/canonical.html")));

	const auto testFunction = [cl = env.crawler()]()
	{
		auto pages = cl->waitForAllCrawledPageReceived(10);
		EXPECT_EQ(2, pages.size());
		EXPECT_EQ(QUrl("http://links.com/canonical-canonical.html"), pages[0]->canonicalUrl);
	};

	env.initializeTest(testFunction);
	env.exec();
}

}