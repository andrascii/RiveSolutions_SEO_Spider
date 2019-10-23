#include "stdafx.h"
#include "tests_crawler.h"

namespace CrawlerEngineTests
{

using namespace CrawlerEngine;

TEST(SearchGaCountersTests, SearchOldGaVer1)
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

TEST(SearchGaCountersTests, SearchOldGaVer2)
{
	TestEnvironment env;
	env.crawler()->options()->setData(TestEnvironment::defaultOptions({ Url("http://gacounters.com/old-ga-ver-2.html") }));
	env.crawler()->options()->setSearchGoogleAnalyticsCounters(true);

	env.crawler()->options()->setSearchGoogleAnalyticsCounter1(true);
	env.crawler()->options()->setGoogleAnalyticsCounter1Id("UA-11111-1");

	const auto testFunction = [cl = env.crawler()]()
	{
		cl->waitForCrawlingDone();

		const std::vector<const ParsedPage*> pagesWithMissingGa1Counter =
			cl->storageItems(StorageType::GoogleAnalyticsCounter1StorageType);


		ASSERT_EQ(0, pagesWithMissingGa1Counter.size());
	};

	env.initializeTest(testFunction);
	env.exec();
}

TEST(SearchGaCountersTests, SearchOldGaVer3)
{
	TestEnvironment env;
	env.crawler()->options()->setData(TestEnvironment::defaultOptions({ Url("http://gacounters.com/old-ga-ver-3.html") }));
	env.crawler()->options()->setSearchGoogleAnalyticsCounters(true);

	env.crawler()->options()->setSearchGoogleAnalyticsCounter1(true);
	env.crawler()->options()->setGoogleAnalyticsCounter1Id("UA-11111-1");

	const auto testFunction = [cl = env.crawler()]()
	{
		cl->waitForCrawlingDone();

		const std::vector<const ParsedPage*> pagesWithMissingGa1Counter =
			cl->storageItems(StorageType::GoogleAnalyticsCounter1StorageType);


		ASSERT_EQ(0, pagesWithMissingGa1Counter.size());
	};

	env.initializeTest(testFunction);
	env.exec();
}

TEST(SearchGaCountersTests, DisableOldGa)
{
	TestEnvironment env;
	env.crawler()->options()->setData(TestEnvironment::defaultOptions({ Url("http://gacounters.com/old-ga-disabled-counter.html") }));
	env.crawler()->options()->setSearchGoogleAnalyticsCounters(true);

	env.crawler()->options()->setSearchGoogleAnalyticsCounter1(true);
	env.crawler()->options()->setGoogleAnalyticsCounter1Id("UA-11111-1");

	const auto testFunction = [cl = env.crawler()]()
	{
		cl->waitForCrawlingDone();

		const std::vector<const ParsedPage*> pagesWithMissingGa1Counter =
			cl->storageItems(StorageType::GoogleAnalyticsCounter1StorageType);


		ASSERT_EQ(1, pagesWithMissingGa1Counter.size());
	};

	env.initializeTest(testFunction);
	env.exec();
}

TEST(SearchGaCountersTests, NewGTAG)
{
	TestEnvironment env;
	env.crawler()->options()->setData(TestEnvironment::defaultOptions({ Url("http://gacounters.com/new-ga-gtag-counter.html") }));
	env.crawler()->options()->setSearchGoogleAnalyticsCounters(true);

	env.crawler()->options()->setSearchGoogleAnalyticsCounter1(true);
	env.crawler()->options()->setGoogleAnalyticsCounter1Id("UA-121704852-1");

	const auto testFunction = [cl = env.crawler()]()
	{
		cl->waitForCrawlingDone();

		const std::vector<const ParsedPage*> pagesWithMissingGa1Counter =
			cl->storageItems(StorageType::GoogleAnalyticsCounter1StorageType);

		ASSERT_EQ(0, pagesWithMissingGa1Counter.size());
	};

	env.initializeTest(testFunction);
	env.exec();
}

TEST(SearchGaCountersTests, NewGA)
{
	TestEnvironment env;
	env.crawler()->options()->setData(TestEnvironment::defaultOptions({ Url("http://gacounters.com/new-ga-gtag-counter.html") }));
	env.crawler()->options()->setSearchGoogleAnalyticsCounters(true);

	env.crawler()->options()->setSearchGoogleAnalyticsCounter1(true);
	env.crawler()->options()->setGoogleAnalyticsCounter1Id("UA-11111-1");

	const auto testFunction = [cl = env.crawler()]()
	{
		cl->waitForCrawlingDone();

		const std::vector<const ParsedPage*> pagesWithMissingGa1Counter =
			cl->storageItems(StorageType::GoogleAnalyticsCounter1StorageType);

		ASSERT_EQ(0, pagesWithMissingGa1Counter.size());
	};

	env.initializeTest(testFunction);
	env.exec();
}

TEST(SearchGaCountersTests, RealPage1)
{
	// source: https://texterra.ru/blog/google-analytics-dlya-nachinayushchikh-samoe-polnoe-rukovodstvo-v-runete.html
	TestEnvironment env;
	env.crawler()->options()->setData(TestEnvironment::defaultOptions({ Url("http://gacounters.com/real-page-1.html") }));
	env.crawler()->options()->setSearchGoogleAnalyticsCounters(true);
	env.crawler()->options()->setLimitSearchTotal(1);

	env.crawler()->options()->setSearchGoogleAnalyticsCounter1(true);
	env.crawler()->options()->setGoogleAnalyticsCounter1Id("UA-31053658-1");

	const auto testFunction = [cl = env.crawler()]()
	{
		cl->waitForCrawlingDone();

		const std::vector<const ParsedPage*> pagesWithMissingGa1Counter =
			cl->storageItems(StorageType::GoogleAnalyticsCounter1StorageType);

		ASSERT_EQ(0, pagesWithMissingGa1Counter.size());
	};

	env.initializeTest(testFunction);
	env.exec();
}


TEST(SearchGaCountersTests, RealPage2)
{
	// source: https://netology.ru/blog/google-analytics-new
	TestEnvironment env;
	env.crawler()->options()->setData(TestEnvironment::defaultOptions({ Url("http://gacounters.com/real-page-2.html") }));
	env.crawler()->options()->setSearchGoogleAnalyticsCounters(true);
	env.crawler()->options()->setLimitSearchTotal(1);

	env.crawler()->options()->setSearchGoogleAnalyticsCounter1(true);
	env.crawler()->options()->setGoogleAnalyticsCounter1Id("GTM-PWG78J");

	const auto testFunction = [cl = env.crawler()]()
	{
		cl->waitForCrawlingDone();

		const std::vector<const ParsedPage*> pagesWithMissingGa1Counter =
			cl->storageItems(StorageType::GoogleAnalyticsCounter1StorageType);

		ASSERT_EQ(0, pagesWithMissingGa1Counter.size());
	};

	env.initializeTest(testFunction);
	env.exec();
}

TEST(SearchGaCountersTests, RealPage3)
{
	// source: https://wiki.rookee.ru/google-analytics/
	TestEnvironment env;
	env.crawler()->options()->setData(TestEnvironment::defaultOptions({ Url("http://gacounters.com/real-page-3.html") }));
	env.crawler()->options()->setSearchGoogleAnalyticsCounters(true);
	env.crawler()->options()->setLimitSearchTotal(1);

	env.crawler()->options()->setSearchGoogleAnalyticsCounter1(true);
	env.crawler()->options()->setGoogleAnalyticsCounter1Id("GTM-P6FZK6R");

	const auto testFunction = [cl = env.crawler()]()
	{
		cl->waitForCrawlingDone();

		const std::vector<const ParsedPage*> pagesWithMissingGa1Counter =
			cl->storageItems(StorageType::GoogleAnalyticsCounter1StorageType);

		ASSERT_EQ(0, pagesWithMissingGa1Counter.size());
	};

	env.initializeTest(testFunction);
	env.exec();
}

TEST(SearchGaCountersTests, RealPage4)
{
	// source: https://convertmonster.ru/blog/kontekstnaya-reklama-blog/ustanovka-schetchika-nastrojka-google-analytics/
	TestEnvironment env;
	env.crawler()->options()->setData(TestEnvironment::defaultOptions({ Url("http://gacounters.com/real-page-4.html") }));
	env.crawler()->options()->setSearchGoogleAnalyticsCounters(true);
	env.crawler()->options()->setLimitSearchTotal(1);

	env.crawler()->options()->setSearchGoogleAnalyticsCounter1(true);
	env.crawler()->options()->setGoogleAnalyticsCounter1Id("GTM-5QSCMZ");

	const auto testFunction = [cl = env.crawler()]()
	{
		cl->waitForCrawlingDone();

		const std::vector<const ParsedPage*> pagesWithMissingGa1Counter =
			cl->storageItems(StorageType::GoogleAnalyticsCounter1StorageType);

		ASSERT_EQ(0, pagesWithMissingGa1Counter.size());
	};

	env.initializeTest(testFunction);
	env.exec();
}

TEST(SearchGaCountersTests, RealPage5)
{
	// source: https://semantica.in/blog/google-analytics.html
	TestEnvironment env;
	env.crawler()->options()->setData(TestEnvironment::defaultOptions({ Url("http://gacounters.com/real-page-5.html") }));
	env.crawler()->options()->setSearchGoogleAnalyticsCounters(true);
	env.crawler()->options()->setLimitSearchTotal(1);

	env.crawler()->options()->setSearchGoogleAnalyticsCounter1(true);
	env.crawler()->options()->setGoogleAnalyticsCounter1Id("UA-70455991-2");

	const auto testFunction = [cl = env.crawler()]()
	{
		cl->waitForCrawlingDone();

		const std::vector<const ParsedPage*> pagesWithMissingGa1Counter =
			cl->storageItems(StorageType::GoogleAnalyticsCounter1StorageType);

		ASSERT_EQ(0, pagesWithMissingGa1Counter.size());
	};

	env.initializeTest(testFunction);
	env.exec();
}

}