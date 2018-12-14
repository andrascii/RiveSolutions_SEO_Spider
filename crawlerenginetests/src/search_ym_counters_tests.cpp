#include "tests_crawler.h"

namespace CrawlerEngineTests
{

using namespace CrawlerEngine;

TEST(SearchYmCountersTests, SearchResults)
{
	TestEnvironment env;
	env.crawler()->options()->setData(TestEnvironment::defaultOptions({ Url("http://ymcounters.com/index.html") }));
	env.crawler()->options()->setSearchYandexMetricaCounters(true);

	env.crawler()->options()->setSearchYandexMetricaCounter1(true);
	env.crawler()->options()->setYandexMetricaCounter1Id(11111111);

	env.crawler()->options()->setSearchYandexMetricaCounter2(true);
	env.crawler()->options()->setYandexMetricaCounter2Id(22222222);

	env.crawler()->options()->setSearchYandexMetricaCounter3(true);
	env.crawler()->options()->setYandexMetricaCounter3Id(33333333);

	env.crawler()->options()->setSearchYandexMetricaCounter4(true);
	env.crawler()->options()->setYandexMetricaCounter4Id(44444444);

	env.crawler()->options()->setSearchYandexMetricaCounter5(true);
	env.crawler()->options()->setYandexMetricaCounter5Id(55555555);

	const auto testFunction = [cl = env.crawler()]()
	{
		cl->waitForCrawlingDone();

		const std::vector<const ParsedPage*> pagesWithMissingYm1Counter =
			cl->storageItems(StorageType::YandexMetricaCounter1StorageType);

		const std::vector<const ParsedPage*> pagesWithMissingYm2Counter =
			cl->storageItems(StorageType::YandexMetricaCounter2StorageType);

		const std::vector<const ParsedPage*> pagesWithMissingYm3Counter =
			cl->storageItems(StorageType::YandexMetricaCounter3StorageType);

		const std::vector<const ParsedPage*> pagesWithMissingYm4Counter =
			cl->storageItems(StorageType::YandexMetricaCounter4StorageType);

		const std::vector<const ParsedPage*> pagesWithMissingYm5Counter =
			cl->storageItems(StorageType::YandexMetricaCounter5StorageType);

		const std::vector<const ParsedPage*> pagesWith301Redirect =
			cl->storageItems(StorageType::Status301StorageType);

		const std::vector<const ParsedPage*> blockedByNofollowAttributePages =
			cl->storageItems(StorageType::NofollowLinksStorageType);


		ASSERT_EQ(1, pagesWithMissingYm1Counter.size());
		ASSERT_EQ(3, pagesWithMissingYm2Counter.size());
		ASSERT_EQ(3, pagesWithMissingYm3Counter.size());
		ASSERT_EQ(3, pagesWithMissingYm4Counter.size());
		ASSERT_EQ(3, pagesWithMissingYm5Counter.size());
		ASSERT_EQ(1, pagesWith301Redirect.size());
		ASSERT_EQ(0, blockedByNofollowAttributePages.size());

		const QString pageWithMissingYmCounter("http://ymcounters.com/index-2.html");
		const QString pageWith301Redirect("http://ymcounters.com/index-4.html");

		ASSERT_EQ(pagesWithMissingYm1Counter[0]->url.toDisplayString(), pageWithMissingYmCounter);
		ASSERT_EQ(pagesWith301Redirect[0]->url.toDisplayString(), pageWith301Redirect);

		const ParsedPage* pageWithMissingYmCounterPointer = pagesWithMissingYm1Counter[0];
		ASSERT_EQ(1, pageWithMissingYmCounterPointer->linksOnThisPage.size());

		ParsedPage* resourceOnPageWithMissingYmCounter = pageWithMissingYmCounterPointer->linksOnThisPage[0].resource.lock().get();
		ASSERT_EQ(resourceOnPageWithMissingYmCounter->url, Url("http://ymcounters.com/index-3.html"));

		ASSERT_EQ(1, pageWithMissingYmCounterPointer->linksToThisPage.size());

		ParsedPage* resourceToPageWithMissingYmCounter = pageWithMissingYmCounterPointer->linksToThisPage[0].resource.lock().get();
		ASSERT_EQ(resourceToPageWithMissingYmCounter->url, Url("http://ymcounters.com/index.html"));
	};

	env.initializeTest(testFunction);
	env.exec();
}

TEST(SearchYmCountersTests, DiscardExternalPages)
{
	TestEnvironment env;
	env.crawler()->options()->setData(TestEnvironment::defaultOptions({ Url("http://ymcounters.com/with-link-to-external-page.html") }));
	env.crawler()->options()->setCheckExternalLinks(true);
	env.crawler()->options()->setSearchYandexMetricaCounters(true);

	env.crawler()->options()->setSearchYandexMetricaCounter1(true);
	env.crawler()->options()->setYandexMetricaCounter1Id(11111111);

	const auto testFunction = [cl = env.crawler()]()
	{
		const auto allPages = cl->waitForAllCrawledPageReceived(10);
		const auto pagesWithMissingYm1Counter = cl->storageItems(StorageType::YandexMetricaCounter1StorageType);

		EXPECT_EQ(allPages.size(), 2);
		EXPECT_EQ(allPages.at(1)->statusCode, Common::StatusCode::Ok200);
		EXPECT_EQ(allPages.at(1)->isThisExternalPage, true);
	
		EXPECT_EQ(pagesWithMissingYm1Counter.size(), 1);
		EXPECT_EQ(pagesWithMissingYm1Counter.at(0)->isThisExternalPage, false);
	};

	env.initializeTest(testFunction);
	env.exec();
}

}