#include "tests_crawler.h"

namespace CrawlerEngineTests
{

using namespace CrawlerEngine;

TEST(SearchGaCountersTests, SearchResults)
{
	TestEnvironment env;
	env.crawler()->options()->setData(TestEnvironment::defaultOptions({ Url("http://gacounters.com/index.html") }));
	env.crawler()->options()->setSearchGoogleAnalyticsCounters(true);

	env.crawler()->options()->setSearchGoogleAnalyticsCounter1(true);
	env.crawler()->options()->setGoogleAnalyticsCounter1Id("UA-11111-1");

	env.crawler()->options()->setSearchGoogleAnalyticsCounter2(true);
	env.crawler()->options()->setGoogleAnalyticsCounter2Id("UA-11111-2");

	env.crawler()->options()->setSearchGoogleAnalyticsCounter3(true);
	env.crawler()->options()->setGoogleAnalyticsCounter3Id("UA-11111-3");

	env.crawler()->options()->setSearchGoogleAnalyticsCounter4(true);
	env.crawler()->options()->setGoogleAnalyticsCounter4Id("UA-11111-4");

	env.crawler()->options()->setSearchGoogleAnalyticsCounter5(true);
	env.crawler()->options()->setGoogleAnalyticsCounter5Id("UA-11111-5");

	const auto testFunction = [cl = env.crawler()]()
	{
		cl->waitForCrawlingDone();

		const std::vector<const ParsedPage*> pagesWithMissingGa1Counter =
			cl->storageItems(StorageType::GoogleAnalyticsCounter1StorageType);

		const std::vector<const ParsedPage*> pagesWithMissingGa2Counter =
			cl->storageItems(StorageType::GoogleAnalyticsCounter2StorageType);

		const std::vector<const ParsedPage*> pagesWithMissingGa3Counter =
			cl->storageItems(StorageType::GoogleAnalyticsCounter3StorageType);

		const std::vector<const ParsedPage*> pagesWithMissingGa4Counter =
			cl->storageItems(StorageType::GoogleAnalyticsCounter4StorageType);

		const std::vector<const ParsedPage*> pagesWithMissingGa5Counter =
			cl->storageItems(StorageType::GoogleAnalyticsCounter5StorageType);

		const std::vector<const ParsedPage*> pagesWith301Redirect =
			cl->storageItems(StorageType::Status301StorageType);

		const std::vector<const ParsedPage*> blockedByNofollowAttributePages =
			cl->storageItems(StorageType::NofollowLinksStorageType);


		ASSERT_EQ(1, pagesWithMissingGa1Counter.size());
		ASSERT_EQ(3, pagesWithMissingGa2Counter.size());
		ASSERT_EQ(3, pagesWithMissingGa3Counter.size());
		ASSERT_EQ(3, pagesWithMissingGa4Counter.size());
		ASSERT_EQ(3, pagesWithMissingGa5Counter.size());
		ASSERT_EQ(1, pagesWith301Redirect.size());
		ASSERT_EQ(0, blockedByNofollowAttributePages.size());

		const QString pageWithMissingGaCounter("http://gacounters.com/index-2.html");
		const QString pageWith301Redirect("http://gacounters.com/index-4.html");

		ASSERT_EQ(pagesWithMissingGa1Counter[0]->url.toDisplayString(), pageWithMissingGaCounter);
		ASSERT_EQ(pagesWith301Redirect[0]->url.toDisplayString(), pageWith301Redirect);

		const ParsedPage* pageWithMissingGaCounterPointer = pagesWithMissingGa1Counter[0];
		ASSERT_EQ(1, pageWithMissingGaCounterPointer->linksOnThisPage.size());

		ParsedPage* resourceOnPageWithMissingGaCounter = pageWithMissingGaCounterPointer->linksOnThisPage[0].resource.lock().get();
		ASSERT_EQ(resourceOnPageWithMissingGaCounter->url, Url("http://gacounters.com/index-3.html"));

		ASSERT_EQ(1, pageWithMissingGaCounterPointer->linksToThisPage.size());

		ParsedPage* resourceToPageWithMissingGaCounter = pageWithMissingGaCounterPointer->linksToThisPage[0].resource.lock().get();
		ASSERT_EQ(resourceToPageWithMissingGaCounter->url, Url("http://gacounters.com/index.html"));
	};

	env.initializeTest(testFunction);
	env.exec();
}

}