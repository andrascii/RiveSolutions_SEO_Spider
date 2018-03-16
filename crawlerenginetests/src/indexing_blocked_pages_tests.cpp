#include "parsed_page.h"

namespace CrawlerEngineTests
{

using namespace CrawlerEngine;

TEST(IndexingBlockedPagesTests, TagARelNofollowTest)
{
	CrawlerOptions options = TestEnvironment::defaultOptions(Url("http://indexingblocked1.com/index.html"));
	options.followInternalNofollow = true;
	options.followRobotsTxtRules = true;
	options.userAgentToFollow = UserAgentType::AnyBot;

	TestEnvironment env(options);

	const auto testFunction = [cl = env.crawler()]()
	{
		const auto pages = cl->waitForParsedPageReceived(CrawlerEngine::StorageType::CrawledUrlStorageType, 3, 10000, "Waiting for 3 crawled page");
		cl->waitForCrawlingDone();
		cl->checkSequencedDataCollectionConsistency();
		const auto nofollowLinks = cl->storageItems(StorageType::NofollowLinksStorageType);

		const Url nofollowBlockedUrl("http://indexingblocked1.com/index-3.html");

		EXPECT_EQ(3, pages.size());
		EXPECT_EQ(1, nofollowLinks.size());

		if (!nofollowLinks.empty())
		{
			EXPECT_EQ(true, nofollowLinks[0]->url == nofollowBlockedUrl);
		}
	};

	env.initializeTest(testFunction);
	env.exec();
}

}