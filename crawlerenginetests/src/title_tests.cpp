#include "stdafx.h"
namespace CrawlerEngineTests
{

using namespace CrawlerEngine;

TEST(TitleTests, EmptyTitle)
{
	// empty-title.html -> empty-title-2.html
	TestEnvironment env;
	env.crawler()->options()->setData(TestEnvironment::defaultOptions({ Url("http://title.com/empty-title.html") }));

	const auto testFunction = [cl = env.crawler()]()
	{
		auto pages = cl->waitForParsedPageReceived(StorageType::EmptyTitleUrlStorageType, 2, 10, "Waiting for 2 empty title pages");
		EXPECT_EQ(2, pages.size());
		EXPECT_EQ(QString(), pages[0]->title);
		EXPECT_EQ(QString(), pages[1]->title);
		cl->waitForParsedPageReceived(StorageType::CrawledUrlStorageType, 2, 10, "Waiting for 2 crawled pages");
	};

	env.initializeTest(testFunction);
	env.exec();
}


TEST(TitleTests, NoTitle)
{
	// no-title.html -> no-title-2.html
	TestEnvironment env;
	env.crawler()->options()->setData(TestEnvironment::defaultOptions({ Url("http://title.com/no-title.html") }));

	const auto testFunction = [cl = env.crawler()]()
	{
		auto pages = cl->waitForParsedPageReceived(StorageType::EmptyTitleUrlStorageType, 2, 10, "Waiting for 2 empty title pages");
		EXPECT_EQ(2, pages.size());
		EXPECT_EQ(QString(), pages[0]->title);
		EXPECT_EQ(QString(), pages[1]->title);
		cl->waitForParsedPageReceived(StorageType::CrawledUrlStorageType, 2, 10, "Waiting for 2 crawled pages");
	};

	env.initializeTest(testFunction);
	env.exec();
}


TEST(TitleTests, TooLongTitle)
{
	// too-long-title.html -> too-long-title-2.html
	TestEnvironment env;

	auto options = TestEnvironment::defaultOptions(Url("http://title.com/too-long-title.html"));
	options.maxTitleLength = 10;

	env.crawler()->options()->setData(options);

	const auto testFunction = [cl = env.crawler()]()
	{
		auto pages = cl->waitForParsedPageReceived(StorageType::TooLongTitleUrlStorageType, 2, 10, "Waiting for 2 too long title pages");
		EXPECT_EQ(2, pages.size());
		EXPECT_EQ(QString("This is too long title"), pages[0]->title);
		EXPECT_EQ(QString("This is too long title"), pages[1]->title);
		cl->waitForParsedPageReceived(StorageType::CrawledUrlStorageType, 2, 10, "Waiting for 2 crawled pages");
	};

	env.initializeTest(testFunction);
	env.exec();
}

TEST(TitleTests, TooShortTitle)
{
	// too-short-title.html -> too-short-title-2.html
	TestEnvironment env;

	auto options = TestEnvironment::defaultOptions(Url("http://title.com/too-short-title.html"));
	options.minTitleLength = 24;
	options.maxTitleLength = 100;
	
	env.crawler()->options()->setData(options);

	const auto testFunction = [cl = env.crawler()]()
	{
		auto pages = cl->waitForParsedPageReceived(StorageType::TooShortTitleUrlStorageType, 2, 10, "Waiting for 2 too short title pages");
		EXPECT_EQ(2, pages.size());
		EXPECT_EQ(QString("This is too short title"), pages[0]->title);
		EXPECT_EQ(QString("This is too short title"), pages[1]->title);
		cl->waitForParsedPageReceived(StorageType::CrawledUrlStorageType, 2, 10, "Waiting for 2 crawled pages");
	};

	env.initializeTest(testFunction);
	env.exec();
}

TEST(TitleTests, DuplicatedTitles)
{
	// duplicated-titles-1.html -> duplicated-titles-2.html
	TestEnvironment env;

	auto options = TestEnvironment::defaultOptions(Url("http://title.com/duplicated-titles-1.html"));
	options.minTitleLength = 24;
	options.maxTitleLength = 100;
	
	env.crawler()->options()->setData(options);

	const auto testFunction = [cl = env.crawler()]()
	{
		cl->waitForAllCrawledPageReceived(10);
		auto pages = cl->storageItems(StorageType::DuplicatedTitleUrlStorageType);
		EXPECT_EQ(2, pages.size());
		EXPECT_EQ(QString("Duplicated Title"), pages[0]->title);
		EXPECT_EQ(QString("Duplicated Title"), pages[1]->title);
		cl->waitForParsedPageReceived(StorageType::CrawledUrlStorageType, 2, 10, "Waiting for 2 crawled pages");
		cl->waitForParsedPageReceived(StorageType::TooShortTitleUrlStorageType, 2, 10, "Waiting for 2 too short title pages");
	};

	env.initializeTest(testFunction);
	env.exec();
}

TEST(TitleTests, DoNotIncludeCanonicalDuplicatedTitles)
{
	// canonical-duplicated-title.html -> canonical-duplicated-title.html
	TestEnvironment env;
	env.crawler()->options()->setData(TestEnvironment::defaultOptions(Url("http://title.com/canonical-duplicated-title.html")));

	const auto testFunction = [cl = env.crawler()]()
	{
		cl->waitForAllCrawledPageReceived(10);
		auto pages = cl->storageItems(StorageType::DuplicatedTitleUrlStorageType);
		EXPECT_EQ(0, pages.size());
	};

	env.initializeTest(testFunction);
	env.exec();
}

TEST(TitleTests, IncludeDuplicatedTitlesIfThereAreSeveralCanonical)
{
	// canonical-duplicated-title-another.html -> canonical-duplicated-title-another-1.html -> canonical-duplicated-title.html -> canonical-duplicated-title.html
	TestEnvironment env;
	env.crawler()->options()->setData(TestEnvironment::defaultOptions(Url("http://title.com/canonical-duplicated-title-another.html")));

	const auto testFunction = [cl = env.crawler()]()
	{
		cl->waitForAllCrawledPageReceived(10);
		auto pages = cl->storageItems(StorageType::DuplicatedTitleUrlStorageType);
		EXPECT_EQ(4, pages.size());
	};

	env.initializeTest(testFunction);
	env.exec();
}

TEST(TitleTests, DoNotIncludeEveryPageInDuplicates)
{
	TestEnvironment env;
	env.crawler()->options()->setData(TestEnvironment::defaultOptions(Url("http://title.com/single-page.html")));

	const auto testFunction = [cl = env.crawler()]()
	{
		cl->waitForAllCrawledPageReceived(10);
		auto pages = cl->storageItems(StorageType::DuplicatedTitleUrlStorageType);
		EXPECT_EQ(0, pages.size());
	};

	env.initializeTest(testFunction);
	env.exec();
}

TEST(TitleTests, DuplicatedH1Title)
{
	// duplicated-h1-title.html -> duplicated-h1-title-2.html
	TestEnvironment env;
	env.crawler()->options()->setData(TestEnvironment::defaultOptions(Url("http://title.com/duplicated-h1-title.html")));

	const auto testFunction = [cl = env.crawler()]()
	{
		auto pages = cl->waitForParsedPageReceived(StorageType::DuplicatedH1TitleUrlStorageType, 2, 10, "Waiting for 2 duplicated h1 title pages");
		EXPECT_EQ(2, pages.size());
		EXPECT_EQ(QString("Duplicated H1 Title"), pages[0]->title);
		EXPECT_EQ(QString("Duplicated H1 Title"), pages[1]->title);

		EXPECT_EQ(QString("Duplicated H1 Title"), pages[0]->firstH1);
		EXPECT_EQ(QString("Duplicated H1 Title"), pages[1]->firstH1);

		cl->waitForParsedPageReceived(StorageType::CrawledUrlStorageType, 2, 10, "Waiting for 2 crawled pages");
	};

	env.initializeTest(testFunction);
	env.exec();
}

TEST(TitleTests, SeveralTitles)
{
	// several-titles.html -> several-titles-2.html
	TestEnvironment env;
	env.crawler()->options()->setData(TestEnvironment::defaultOptions(Url("http://title.com/several-titles.html")));

	const auto testFunction = [cl = env.crawler()]()
	{
		auto pages = cl->waitForParsedPageReceived(StorageType::SeveralTitleUrlStorageType, 2, 10, "Waiting for 2 several titles pages");
		EXPECT_EQ(2, pages.size());
		EXPECT_EQ(QString("First Title"), pages[0]->title);
		EXPECT_EQ(QString("First Title"), pages[1]->title);
		cl->waitForParsedPageReceived(StorageType::CrawledUrlStorageType, 2, 10, "Waiting for 2 crawled pages");
	};

	env.initializeTest(testFunction);
	env.exec();
}

TEST(TitleTests, ShinnyiMir)
{
	// several-titles.html -> several-titles-2.html
	TestEnvironment env;
	env.crawler()->options()->setData(TestEnvironment::defaultOptions(Url("http://title.com/shinnyi-mir.html")));

	const auto testFunction = [cl = env.crawler()]()
	{
		//auto pages = cl->waitForParsedPageReceived(StorageType::CrawledUrlStorageType, 1, 10, "Waiting for 1 page");
		auto pages = cl->waitForAllCrawledPageReceived(10);
		EXPECT_EQ(1, pages.size());
		EXPECT_EQ(false, pages[0]->title.isEmpty());
	};

	env.initializeTest(testFunction);
	env.exec();
}

}
