#include "parsed_page.h"

namespace CrawlerEngineTests
{

using namespace CrawlerEngine;

TEST(IndexingBlockedPagesTests, TagARelNofollowTest)
{
	CrawlerOptions options = TestEnvironment::defaultOptions(Url("http://indexingblocked1.com/index.html"));
	options.followInternalNofollow = true;

	TestEnvironment env(options);

	const auto testFunction = [cl = env.crawler()]()
	{
		const auto pages = cl->waitForParsedPageReceived(CrawlerEngine::StorageType::CrawledUrlStorageType, 3, 10, "Waiting for 3 crawled page");
		cl->waitForCrawlingDone();
		cl->checkSequencedDataCollectionConsistency();
		const auto nofollowLinks = cl->storageItems(StorageType::NofollowLinksStorageType);
		const auto blockedForSeIndexing = cl->storageItems(StorageType::BlockedForSEIndexingStorageType);

		const Url nofollowBlockedUrl("http://indexingblocked1.com/index-3.html");

		EXPECT_EQ(3, pages.size());
		EXPECT_EQ(1, nofollowLinks.size());
		EXPECT_EQ(1, blockedForSeIndexing.size());

		if (!nofollowLinks.empty())
		{
			EXPECT_EQ(true, nofollowLinks[0]->url == nofollowBlockedUrl);
			EXPECT_EQ(true, nofollowLinks[0]->isBlockedForIndexing);
		}

		if (!blockedForSeIndexing.empty())
		{
			EXPECT_EQ(true, blockedForSeIndexing[0]->isBlockedForIndexing);
		}
	};

	env.initializeTest(testFunction);
	env.exec();
}

TEST(IndexingBlockedPagesTests, TagARelNofollowAndCommonMetaRobotsTest)
{
	CrawlerOptions options = TestEnvironment::defaultOptions(Url("http://indexingblocked2.com/index.html"));
	options.followInternalNofollow = true;
	options.followRobotsTxtRules = true;
	options.userAgentToFollow = UserAgentType::AnyBot;

	TestEnvironment env(options);

	const auto testFunction = [cl = env.crawler()]()
	{
		const auto pages = cl->waitForParsedPageReceived(CrawlerEngine::StorageType::CrawledUrlStorageType, 3, 10, "Waiting for 3 crawled page");
		cl->waitForCrawlingDone();
		cl->checkSequencedDataCollectionConsistency();
		const auto nofollowLinks = cl->storageItems(StorageType::NofollowLinksStorageType);
		const auto blockedForSeIndexing = cl->storageItems(StorageType::BlockedForSEIndexingStorageType);
		const auto blockedByXRobotsTag = cl->storageItems(StorageType::BlockedByXRobotsTagStorageType);

		const Url nofollowBlockedUrl("http://indexingblocked2.com/index-3.html");

		EXPECT_EQ(3, pages.size());
		EXPECT_EQ(1, nofollowLinks.size());
		EXPECT_EQ(1, blockedForSeIndexing.size());
		EXPECT_EQ(1, blockedByXRobotsTag.size());

		if (!nofollowLinks.empty())
		{
			EXPECT_EQ(true, nofollowLinks[0]->url == nofollowBlockedUrl);
			EXPECT_EQ(true, nofollowLinks[0]->isBlockedForIndexing);
		}

		if (!blockedForSeIndexing.empty())
		{
			EXPECT_EQ(true, blockedForSeIndexing[0]->isBlockedForIndexing);
		}

		if (!blockedByXRobotsTag.empty())
		{
			EXPECT_EQ(true, blockedByXRobotsTag[0]->isBlockedForIndexing);
		}
	};

	env.initializeTest(testFunction);
	env.exec();
}

TEST(IndexingBlockedPagesTests, TagARelNofollowAndSpecialMetaRobotsTest)
{
	CrawlerOptions options = TestEnvironment::defaultOptions(Url("http://indexingblocked2.com/index.html"));
	options.followInternalNofollow = true;
	options.followRobotsTxtRules = true;
	options.userAgentToFollow = UserAgentType::GoogleBot;

	TestEnvironment env(options);

	const auto testFunction = [cl = env.crawler()]()
	{
		const auto pages = cl->waitForParsedPageReceived(CrawlerEngine::StorageType::CrawledUrlStorageType, 3, 10, "Waiting for 3 crawled page");
		cl->waitForCrawlingDone();
		cl->checkSequencedDataCollectionConsistency();
		const auto nofollowLinks = cl->storageItems(StorageType::NofollowLinksStorageType);
		const auto blockedForSeIndexing = cl->storageItems(StorageType::BlockedForSEIndexingStorageType);
		const auto blockedByXRobotsTag = cl->storageItems(StorageType::BlockedByXRobotsTagStorageType);

		const Url nofollowBlockedUrl("http://indexingblocked2.com/index-3.html");

		EXPECT_EQ(3, pages.size());
		EXPECT_EQ(1, nofollowLinks.size());
		EXPECT_EQ(1, blockedForSeIndexing.size());
		EXPECT_EQ(1, blockedByXRobotsTag.size());

		if (!nofollowLinks.empty())
		{
			EXPECT_EQ(true, nofollowLinks[0]->url == nofollowBlockedUrl);
			EXPECT_EQ(true, nofollowLinks[0]->isBlockedForIndexing);
		}

		if (!blockedForSeIndexing.empty())
		{
			EXPECT_EQ(true, blockedForSeIndexing[0]->isBlockedForIndexing);
		}

		if (!blockedByXRobotsTag.empty())
		{
			EXPECT_EQ(true, blockedByXRobotsTag[0]->isBlockedForIndexing);
		}
	};

	env.initializeTest(testFunction);
	env.exec();
}

TEST(IndexingBlockedPagesTests, TagARelNofollowAndRobotsTxtTest)
{
	CrawlerOptions options = TestEnvironment::defaultOptions(Url("http://indexingblocked3.com/index.html"));
	options.followInternalNofollow = true;
	options.followRobotsTxtRules = true;
	options.userAgentToFollow = UserAgentType::AnyBot;

	TestEnvironment env(options);

	const auto testFunction = [cl = env.crawler()]()
	{
		const auto pages = cl->waitForParsedPageReceived(CrawlerEngine::StorageType::CrawledUrlStorageType, 3, 10, "Waiting for 3 crawled page");
		cl->waitForCrawlingDone();
		cl->checkSequencedDataCollectionConsistency();
		const auto nofollowLinks = cl->storageItems(StorageType::NofollowLinksStorageType);
		const auto blockedForSeIndexing = cl->storageItems(StorageType::BlockedForSEIndexingStorageType);
		const auto blockedByXRobotsTag = cl->storageItems(StorageType::BlockedByXRobotsTagStorageType);
		const auto blockedByRobotsTxt = cl->storageItems(StorageType::BlockedByRobotsTxtStorageType);

		const Url nofollowBlockedUrl("http://indexingblocked3.com/index-3.html");

		EXPECT_EQ(3, pages.size());
		EXPECT_EQ(1, nofollowLinks.size());
		EXPECT_EQ(1, blockedForSeIndexing.size());
		EXPECT_EQ(1, blockedByXRobotsTag.size());
		EXPECT_EQ(1, blockedByRobotsTxt.size());

		if (!nofollowLinks.empty())
		{
			EXPECT_EQ(true, nofollowLinks[0]->url == nofollowBlockedUrl);
			EXPECT_EQ(true, nofollowLinks[0]->isBlockedForIndexing);
		}

		if (!blockedForSeIndexing.empty())
		{
			EXPECT_EQ(true, blockedForSeIndexing[0]->isBlockedForIndexing);
		}

		if (!blockedByXRobotsTag.empty())
		{
			EXPECT_EQ(true, blockedByXRobotsTag[0]->isBlockedForIndexing);
		}
	};

	env.initializeTest(testFunction);
	env.exec();
}

}