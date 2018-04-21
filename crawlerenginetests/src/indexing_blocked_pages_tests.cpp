#include "parsed_page.h"

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
			EXPECT_EQ(true, nofollowLinks[0]->isBlockedForIndexing);
		}

		if (!blockedForSeIndexing.empty())
		{
			EXPECT_EQ(true, nofollowLinks[0]->url == nofollowBlockedUrl);
			EXPECT_EQ(true, nofollowLinks[0]->isBlockedForIndexing);
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
			EXPECT_EQ(true, nofollowLinks[0]->isBlockedForIndexing);
		}

		if (!blockedForSeIndexing.empty())
		{
			EXPECT_EQ(true, nofollowLinks[0]->url == nofollowBlockedUrl);
			EXPECT_EQ(true, nofollowLinks[0]->isBlockedForIndexing);
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
			EXPECT_EQ(true, nofollowLinks[0]->isBlockedForIndexing);
		}

		if (!blockedForSeIndexing.empty())
		{
			EXPECT_EQ(true, nofollowLinks[0]->url == nofollowBlockedUrl);
			EXPECT_EQ(true, nofollowLinks[0]->isBlockedForIndexing);
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

TEST(IndexingBlockedPagesTests, TagARelNofollowAndDofollowTest)
{
	//
	// This test case checks behavior for index-2.html, which is dofollow and on another page it is nofollow
	// and only index-3.html actually nofollow link on all other pages
	//

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
		}

		if (!blockedForSeIndexing.empty())
		{
			EXPECT_EQ(true, blockedForSeIndexing[0]->url == nofollowBlockedUrl);
			EXPECT_EQ(true, blockedForSeIndexing[0]->isBlockedForIndexing);
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
			EXPECT_EQ(true, blockedForSeIndexing[0]->isBlockedForIndexing);
			EXPECT_EQ(true, blockedForSeIndexing[0]->isBlockedByMetaRobots);
			EXPECT_EQ(true, blockedForSeIndexing[0]->url.canonizedUrlStr() == xRobotsTagBlockedUrl.canonizedUrlStr());
		}

		if (!blockedByXRobotsTag.empty())
		{
			EXPECT_EQ(true, blockedByXRobotsTag[0]->isBlockedForIndexing);
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
	// But index-3.html must not be blocked.
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
			EXPECT_EQ(true, blockedForSeIndexing[0]->isBlockedForIndexing);
			EXPECT_EQ(true, blockedForSeIndexing[0]->isBlockedByMetaRobots);
			EXPECT_EQ(true, blockedForSeIndexing[0]->url.canonizedUrlStr() == xRobotsTagBlockedUrl.canonizedUrlStr());
		}

		if (!blockedByXRobotsTag.empty())
		{
			EXPECT_EQ(true, blockedByXRobotsTag[0]->isBlockedForIndexing);
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
			EXPECT_EQ(true, blockedForSeIndexing[0]->isBlockedForIndexing);
			EXPECT_EQ(true, blockedForSeIndexing[0]->isBlockedByMetaRobots);
			EXPECT_EQ(true, blockedForSeIndexing[0]->url.canonizedUrlStr() == xRobotsTagBlockedUrl.canonizedUrlStr());
		}

		if (!blockedByXRobotsTag.empty())
		{
			EXPECT_EQ(true, blockedByXRobotsTag[0]->isBlockedForIndexing);
			EXPECT_EQ(true, blockedByXRobotsTag[0]->isBlockedByMetaRobots);
			EXPECT_EQ(true, blockedByXRobotsTag[0]->url.canonizedUrlStr() == xRobotsTagBlockedUrl.canonizedUrlStr());
		}

		EXPECT_EQ(true, blockedByXRobotsTag == blockedForSeIndexing);
	};

	env.initializeTest(testFunction);
	env.exec();
}


}