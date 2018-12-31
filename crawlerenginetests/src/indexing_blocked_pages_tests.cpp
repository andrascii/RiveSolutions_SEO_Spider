#include "parsed_page.h"

/*** We have 3 independent storages:
 *		Blocked by robots.txt
 *		Blocked by XRobotsTag (by meta robots)
 *		Blocked by nofollow
 *
 *	The apropriate behaviour for now is to process them separately.
 *	That means that XRobots tags knows nothing about robots.txt and nofollow.
 *	The same is also for robots.txt and XRobots.
 *	So, if the page is blocked by robots.txt or by XRobots but have dofollow links
 *	we remove these links from the 'nofollow' list.
 *	If a user will disable the option 'use robotx.txt rules', he'll see tha same result
 *	of 'nofollow' links as it was when this option was enabled.
 *
 *	!But: metarobots flag nofollow and the corresponding rules for all links on such page
 *	we are treating as a part of the 'Blocked by nofollow' strategy
*/

namespace
{

template <typename Iter, typename T, typename Predicate>
bool containsAll(Iter first, Iter last, const std::vector<T>& values, Predicate&& pred)
{
	bool result = false;

	for(; first != last; ++first)
	{
		result = false;

		for (const T& value : values)
		{
			if (pred(first, value))
			{
				result = true;
				break;
			}
		}

		if (!result)
		{
			return false;
		}
	}

	return result;
}

}

namespace CrawlerEngineTests
{

using namespace CrawlerEngine;

TEST(IndexingBlockedPagesTests, TagARelNofollowTest)
{
	//
	// This test check behavior for nofollow index-3.html page
	// which is nofollow on all pages
	//

	TestEnvironment env;

	CrawlerOptionsData options = TestEnvironment::defaultOptions(Url("http://indexingblocked1.com/index.html"));
	options.followInternalNofollow = true;

	env.crawler()->options()->setData(options);

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
			EXPECT_EQ(true, nofollowLinks[0]->storages[StorageType::BlockedForSEIndexingStorageType]);
		}

		if (!blockedForSeIndexing.empty())
		{
			EXPECT_EQ(true, blockedForSeIndexing[0]->storages[StorageType::BlockedForSEIndexingStorageType]);
		}
	};

	env.initializeTest(testFunction);
	env.exec();
}

TEST(IndexingBlockedPagesTests, TagARelNofollowAndAnyBotNofollowMetaRobotsTest)
{
	//
	// This test check behavior for nofollow index-3.html page
	// which is nofollow on all pages and index-2.html which
	// is blocked by meta robots using content="nofollow".
	// So this index-2.html page MUST NOT BE CONTAINED
	// in the StorageType::BlockedByXRobotsTagStorageType and StorageType::BlockedForSEIndexingStorageType
	//

	TestEnvironment env;

	auto options = TestEnvironment::defaultOptions(Url("http://indexingblocked2.com/index.html"));
	options.followInternalNofollow = true;
	options.followRobotsTxtRules = true;
	options.userAgentToFollow = UserAgentType::AnyBot;

	env.crawler()->options()->setData(options);

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
		EXPECT_EQ(0, blockedByXRobotsTag.size());

		if (!nofollowLinks.empty())
		{
			EXPECT_EQ(true, nofollowLinks[0]->url == nofollowBlockedUrl);
			EXPECT_EQ(true, nofollowLinks[0]->storages[StorageType::BlockedForSEIndexingStorageType]);
		}

		if (!blockedForSeIndexing.empty())
		{
			EXPECT_EQ(true, nofollowLinks[0]->url == nofollowBlockedUrl);
			EXPECT_EQ(true, nofollowLinks[0]->storages[StorageType::BlockedForSEIndexingStorageType]);
		}
	};

	env.initializeTest(testFunction);
	env.exec();
}

TEST(IndexingBlockedPagesTests, TagARelNofollowAndGoogleNofollowMetaRobotsTest)
{
	//
	// This test check behavior for nofollow index-3.html page
	// which is nofollow on all pages and index-2.html which
	// is blocked by meta robots using content="nofollow".
	// So this index-2.html page MUST NOT BE CONTAINED
	// in the StorageType::BlockedByXRobotsTagStorageType and StorageType::BlockedForSEIndexingStorageType
	//

	TestEnvironment env;

	auto options = TestEnvironment::defaultOptions(Url("http://indexingblocked2.com/index.html"));
	options.followInternalNofollow = true;
	options.followRobotsTxtRules = true;
	options.userAgentToFollow = UserAgentType::GoogleBot;

	env.crawler()->options()->setData(options);

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
		EXPECT_EQ(0, blockedByXRobotsTag.size());

		if (!nofollowLinks.empty())
		{
			EXPECT_EQ(true, nofollowLinks[0]->url == nofollowBlockedUrl);
			EXPECT_EQ(true, nofollowLinks[0]->storages[StorageType::BlockedForSEIndexingStorageType]);
		}

		if (!blockedForSeIndexing.empty())
		{
			EXPECT_EQ(true, nofollowLinks[0]->url == nofollowBlockedUrl);
			EXPECT_EQ(true, nofollowLinks[0]->storages[StorageType::BlockedForSEIndexingStorageType]);
		}
	};

	env.initializeTest(testFunction);
	env.exec();
}

TEST(IndexingBlockedPagesTests, TagARelNofollowAndYandexNofollowMetaRobotsTest)
{
	//
	// This test check behavior for nofollow index-3.html page
	// which is nofollow on all pages and index-2.html which
	// is blocked by meta robots using content="nofollow".
	// So this index-2.html page MUST NOT BE CONTAINED
	// in the StorageType::BlockedByXRobotsTagStorageType and StorageType::BlockedForSEIndexingStorageType
	//

	TestEnvironment env;

	auto options = TestEnvironment::defaultOptions(Url("http://indexingblocked2.com/index.html"));
	options.followInternalNofollow = true;
	options.followRobotsTxtRules = true;
	options.userAgentToFollow = UserAgentType::YandexBot;

	env.crawler()->options()->setData(options);

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
		EXPECT_EQ(0, blockedByXRobotsTag.size());

		if (!nofollowLinks.empty())
		{
			EXPECT_EQ(true, nofollowLinks[0]->url == nofollowBlockedUrl);
			EXPECT_EQ(true, nofollowLinks[0]->storages[StorageType::BlockedForSEIndexingStorageType]);
		}

		if (!blockedForSeIndexing.empty())
		{
			EXPECT_EQ(true, nofollowLinks[0]->url == nofollowBlockedUrl);
			EXPECT_EQ(true, nofollowLinks[0]->storages[StorageType::BlockedForSEIndexingStorageType]);
		}
	};

	env.initializeTest(testFunction);
	env.exec();
}

TEST(IndexingBlockedPagesTests, TagARelNofollowAndRobotsTxtTest)
{
	TestEnvironment env;

	auto options = TestEnvironment::defaultOptions(Url("http://indexingblocked3.com/index.html"));
	options.followInternalNofollow = true;
	options.followRobotsTxtRules = true;
	options.userAgentToFollow = UserAgentType::AnyBot;

	env.crawler()->options()->setData(options);

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
		EXPECT_EQ(0, blockedByXRobotsTag.size());
		EXPECT_EQ(1, blockedByRobotsTxt.size());

		if (!nofollowLinks.empty())
		{
			EXPECT_EQ(true, nofollowLinks[0]->url == nofollowBlockedUrl);
			EXPECT_EQ(true, nofollowLinks[0]->storages[StorageType::BlockedForSEIndexingStorageType]);
		}

		if (!blockedForSeIndexing.empty())
		{
			EXPECT_EQ(true, blockedForSeIndexing[0]->storages[StorageType::BlockedForSEIndexingStorageType]);
		}
	};

	env.initializeTest(testFunction);
	env.exec();
}

TEST(IndexingBlockedPagesTests, TagARelNofollowAndDofollowTest)
{
	//
	// This test case checks behavior for index-2.html, which is nofollow and on another page it is dofollow
	// and only index-3.html actually nofollow link on all other pages
	// Also index-2.html resource from index.html and index-3.html should be merged even though it's initially nofollow

	TestEnvironment env;

	auto options = TestEnvironment::defaultOptions(Url("http://indexingblocked4.com/index.html"));
	options.followInternalNofollow = true;

	env.crawler()->options()->setData(options);

	const auto testFunction = [cl = env.crawler()]()
	{
		const auto pages = cl->waitForParsedPageReceived(CrawlerEngine::StorageType::CrawledUrlStorageType, 3, 10, "Waiting for 3 crawled page");
		cl->waitForCrawlingDone();
		cl->checkSequencedDataCollectionConsistency();

		const auto nofollowLinks = cl->storageItems(StorageType::NofollowLinksStorageType);
		const auto blockedForSeIndexing = cl->storageItems(StorageType::BlockedForSEIndexingStorageType);

		const Url nofollowBlockedUrl("http://indexingblocked4.com/index-3.html");

		EXPECT_EQ(3, pages.size());
		EXPECT_EQ(1, nofollowLinks.size());
		EXPECT_EQ(1, blockedForSeIndexing.size());

		if (!nofollowLinks.empty())
		{
			EXPECT_EQ(true, nofollowLinks[0]->url == nofollowBlockedUrl);
			EXPECT_EQ(2, nofollowLinks[0]->linksToThisPage.size());
		}

		if (!blockedForSeIndexing.empty())
		{
			EXPECT_EQ(true, blockedForSeIndexing[0]->url == nofollowBlockedUrl);
			EXPECT_EQ(true, blockedForSeIndexing[0]->storages[StorageType::BlockedForSEIndexingStorageType]);
		}
	};

	env.initializeTest(testFunction);
	env.exec();
}

TEST(IndexingBlockedPagesTests, TagARelNofollowAndAnyBotNoindexMetaRobotsTest)
{
	//
	// This test check behavior for nofollow index-3.html page
	// which is nofollow on all pages except index-2.html which
	// is blocked by meta robots using content="noindex".
	// So this index-2.html page must be contained
	// in the StorageType::BlockedByXRobotsTagStorageType and StorageType::BlockedForSEIndexingStorageType.
	// But index-3.html must not be blocked.
	//

	TestEnvironment env;

	auto options = TestEnvironment::defaultOptions(Url("http://indexingblocked5.com/index.html"));
	options.followInternalNofollow = true;
	options.followRobotsTxtRules = true;
	options.userAgentToFollow = UserAgentType::AnyBot;

	env.crawler()->options()->setData(options);

	const auto testFunction = [cl = env.crawler()]()
	{
		const auto pages = cl->waitForParsedPageReceived(CrawlerEngine::StorageType::CrawledUrlStorageType, 3, 10, "Waiting for 3 crawled page");
		cl->waitForCrawlingDone();
		cl->checkSequencedDataCollectionConsistency();

		const auto nofollowLinks = cl->storageItems(StorageType::NofollowLinksStorageType);
		const auto blockedForSeIndexing = cl->storageItems(StorageType::BlockedForSEIndexingStorageType);
		const auto blockedByXRobotsTag = cl->storageItems(StorageType::BlockedByXRobotsTagStorageType);

		const Url xRobotsTagBlockedUrl("http://indexingblocked5.com/index-2.html");

		EXPECT_EQ(3, pages.size());
		EXPECT_EQ(0, nofollowLinks.size());
		EXPECT_EQ(1, blockedForSeIndexing.size());
		EXPECT_EQ(1, blockedByXRobotsTag.size());

		if (!blockedForSeIndexing.empty())
		{
			EXPECT_EQ(true, blockedForSeIndexing[0]->storages[StorageType::BlockedForSEIndexingStorageType]);
			EXPECT_EQ(true, blockedForSeIndexing[0]->isBlockedByMetaRobots);
			EXPECT_EQ(true, blockedForSeIndexing[0]->url.canonizedUrlStr() == xRobotsTagBlockedUrl.canonizedUrlStr());
		}

		if (!blockedByXRobotsTag.empty())
		{
			EXPECT_EQ(true, blockedByXRobotsTag[0]->storages[StorageType::BlockedForSEIndexingStorageType]);
			EXPECT_EQ(true, blockedByXRobotsTag[0]->isBlockedByMetaRobots);
			EXPECT_EQ(true, blockedByXRobotsTag[0]->url.canonizedUrlStr() == xRobotsTagBlockedUrl.canonizedUrlStr());
		}
	};

	env.initializeTest(testFunction);
	env.exec();
}

TEST(IndexingBlockedPagesTests, TagARelNofollowAndGoogleNoindexMetaRobotsTest)
{
	//
	// This test check behavior for nofollow index-3.html page
	// which is nofollow on all pages except index-2.html which
	// is blocked by meta robots using content="noindex".
	// So this index-2.html page must be contained
	// in the StorageType::BlockedByXRobotsTagStorageType and StorageType::BlockedForSEIndexingStorageType.
	// index-3.html must not be blocked.
	//

	TestEnvironment env;

	auto options = TestEnvironment::defaultOptions(Url("http://indexingblocked5.com/index.html"));
	options.followInternalNofollow = true;
	options.followRobotsTxtRules = true;
	options.userAgentToFollow = UserAgentType::GoogleBot;

	env.crawler()->options()->setData(options);

	const auto testFunction = [cl = env.crawler()]()
	{
		const auto pages = cl->waitForParsedPageReceived(CrawlerEngine::StorageType::CrawledUrlStorageType, 3, 10, "Waiting for 3 crawled page");
		cl->waitForCrawlingDone();
		cl->checkSequencedDataCollectionConsistency();

		const auto nofollowLinks = cl->storageItems(StorageType::NofollowLinksStorageType);
		const auto blockedForSeIndexing = cl->storageItems(StorageType::BlockedForSEIndexingStorageType);
		const auto blockedByXRobotsTag = cl->storageItems(StorageType::BlockedByXRobotsTagStorageType);

		const Url xRobotsTagBlockedUrl("http://indexingblocked5.com/index-2.html");

		EXPECT_EQ(3, pages.size());
		EXPECT_EQ(0, nofollowLinks.size());
		EXPECT_EQ(1, blockedForSeIndexing.size());
		EXPECT_EQ(1, blockedByXRobotsTag.size());

		if (!blockedForSeIndexing.empty())
		{
			EXPECT_EQ(true, blockedForSeIndexing[0]->storages[StorageType::BlockedForSEIndexingStorageType]);
			EXPECT_EQ(true, blockedForSeIndexing[0]->isBlockedByMetaRobots);
			EXPECT_EQ(true, blockedForSeIndexing[0]->url.canonizedUrlStr() == xRobotsTagBlockedUrl.canonizedUrlStr());
		}

		if (!blockedByXRobotsTag.empty())
		{
			EXPECT_EQ(true, blockedByXRobotsTag[0]->storages[StorageType::BlockedForSEIndexingStorageType]);
			EXPECT_EQ(true, blockedByXRobotsTag[0]->isBlockedByMetaRobots);
			EXPECT_EQ(true, blockedByXRobotsTag[0]->url.canonizedUrlStr() == xRobotsTagBlockedUrl.canonizedUrlStr());
		}
	};

	env.initializeTest(testFunction);
	env.exec();
}

TEST(IndexingBlockedPagesTests, TagARelNofollowAndYandexNoindexMetaRobotsTest)
{
	//
	// This test check behavior for nofollow index-3.html page
	// which is nofollow on all pages except index-2.html which
	// is blocked by meta robots using content="noindex".
	// So this index-2.html page must be contained
	// in the StorageType::BlockedByXRobotsTagStorageType and StorageType::BlockedForSEIndexingStorageType.
	// But index-3.html must not be blocked.
	//

	TestEnvironment env;

	auto options = TestEnvironment::defaultOptions(Url("http://indexingblocked5.com/index.html"));
	options.followInternalNofollow = true;
	options.followRobotsTxtRules = true;
	options.userAgentToFollow = UserAgentType::YandexBot;

	env.crawler()->options()->setData(options);

	const auto testFunction = [cl = env.crawler()]()
	{
		const auto pages = cl->waitForParsedPageReceived(CrawlerEngine::StorageType::CrawledUrlStorageType, 3, 10, "Waiting for 3 crawled page");
		cl->waitForCrawlingDone();
		cl->checkSequencedDataCollectionConsistency();

		const auto nofollowLinks = cl->storageItems(StorageType::NofollowLinksStorageType);
		const auto blockedForSeIndexing = cl->storageItems(StorageType::BlockedForSEIndexingStorageType);
		const auto blockedByXRobotsTag = cl->storageItems(StorageType::BlockedByXRobotsTagStorageType);

		const Url xRobotsTagBlockedUrl("http://indexingblocked5.com/index-2.html");

		EXPECT_EQ(3, pages.size());
		EXPECT_EQ(0, nofollowLinks.size());
		EXPECT_EQ(1, blockedForSeIndexing.size());
		EXPECT_EQ(1, blockedByXRobotsTag.size());

		if (!blockedForSeIndexing.empty())
		{
			EXPECT_EQ(true, blockedForSeIndexing[0]->storages[StorageType::BlockedForSEIndexingStorageType]);
			EXPECT_EQ(true, blockedForSeIndexing[0]->isBlockedByMetaRobots);
			EXPECT_EQ(true, blockedForSeIndexing[0]->url.canonizedUrlStr() == xRobotsTagBlockedUrl.canonizedUrlStr());
		}

		if (!blockedByXRobotsTag.empty())
		{
			EXPECT_EQ(true, blockedByXRobotsTag[0]->storages[StorageType::BlockedForSEIndexingStorageType]);
			EXPECT_EQ(true, blockedByXRobotsTag[0]->isBlockedByMetaRobots);
			EXPECT_EQ(true, blockedByXRobotsTag[0]->url.canonizedUrlStr() == xRobotsTagBlockedUrl.canonizedUrlStr());
		}

		EXPECT_EQ(true, blockedByXRobotsTag == blockedForSeIndexing);
	};

	env.initializeTest(testFunction);
	env.exec();
}


}