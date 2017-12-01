namespace CrawlerEngineTests
{
    
TEST(LinksTests, LinkAlt)
{
    TestEnvironment env(TestEnvironment::defaultOptions(QUrl("http://links.com/link-with-title.html")));

    const auto testFunction = [cl = env.crawler()]()
    {
        auto pages = cl->waitForParsedPageReceived(CrawlerEngine::CrawledUrlStorageType, 2, 10, "Waiting for 2 crawled pages");
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
    TestEnvironment env(TestEnvironment::defaultOptions(QUrl("http://links.com/canonical-next-prev.html")));

    const auto testFunction = [cl = env.crawler()]()
    {
        auto pages = cl->waitForParsedPageReceived(CrawlerEngine::CrawledUrlStorageType, 4, 10, "Waiting for 4 crawled pages");
        cl->checkSequencedDataCollectionConsistency();
        EXPECT_EQ(4, pages.size());

        auto canonical = pages[0]->linksOnThisPage[0];
        auto next = pages[0]->linksOnThisPage[1];
        auto prev = pages[0]->linksOnThisPage[2];

        EXPECT_EQ(QString("http://links.com/next-prev-canonical.html"), canonical.resource.lock()->url.toDisplayString());
        EXPECT_EQ(QString("http://links.com/next.html"), next.resource.lock()->url.toDisplayString());
        EXPECT_EQ(QString("http://links.com/prev.html"), prev.resource.lock()->url.toDisplayString());

        EXPECT_EQ(CrawlerEngine::ResourceSource::SourceTagLinkRelCanonical, canonical.resourceSource);
        EXPECT_EQ(CrawlerEngine::ResourceSource::SourceTagLinkRelNext, next.resourceSource);
        EXPECT_EQ(CrawlerEngine::ResourceSource::SourceTagLinkRelPrev, prev.resourceSource);
    };

    env.initializeTest(testFunction);
    env.exec();
}

TEST(LinksTests, NofollowLinksMustNotBeLoaded)
{
    CrawlerEngine::CrawlerOptions options = TestEnvironment::defaultOptions(QUrl("http://nofollowlinks.com"));
    options.followInternalNofollow = false;

    TestEnvironment env(options);

    const auto testFunction = [cl = env.crawler()]()
    {
        auto pages = cl->waitForAllCrawledPageReceived(10);
        cl->checkSequencedDataCollectionConsistency();

        std::vector<const CrawlerEngine::ParsedPage*> crawledPages = cl->storageItems(CrawlerEngine::CrawledUrlStorageType);
        std::vector<const CrawlerEngine::ParsedPage*> pendingPages = cl->getLinksFromUnorderedDataCollection(CrawlerEngine::PendingResourcesStorageType);

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

        // collect nofollow links
        std::for_each(crawledPages.begin(), crawledPages.end(), collectNofollowLinks);

        // check that pending does not contain nofollow links
        const auto checkPendingLinks = [&nofollowLinks](auto* page)
        {
            for (const QUrl& nofollowLink : nofollowLinks)
            {
                EXPECT_NE(nofollowLink, page->url);
            }
        };
        
        std::for_each(pendingPages.begin(), pendingPages.end(), checkPendingLinks);
    };

    env.initializeTest(testFunction);
    env.exec();
}

TEST(LinksTests, SubdomainsMustNotBeLoaded)
{
    TestEnvironment env(TestEnvironment::defaultOptions(QUrl("http://subdomains.com")));

    const auto testFunction = [cl = env.crawler()]()
    {
        
    };

    env.initializeTest(testFunction);
    env.exec();
}

TEST(LinksTests, BlockedByRobotsTxtLinksMustNotBeLoaded)
{
    TestEnvironment env(TestEnvironment::defaultOptions(QUrl("http://blockedbyrobotstxt.com")));

    const auto testFunction = [cl = env.crawler()]()
    {
        
    };

    env.initializeTest(testFunction);
    env.exec();
}

TEST(LinksTests, Canonical)
{
    TestEnvironment env(TestEnvironment::defaultOptions(QUrl("http://links.com/canonical.html")));

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