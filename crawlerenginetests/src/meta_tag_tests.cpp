#include "stdafx.h"
namespace CrawlerEngineTests
{

using namespace CrawlerEngine;

TEST(MetaTagTests, EmptyMetaDescription)
{
	// empty-meta.html -> empty-meta-2.html
	TestEnvironment env;
	env.crawler()->options()->setData(TestEnvironment::defaultOptions({ Url("http://meta-desc.com/empty-meta.html") }));

	const auto testFunction = [cl = env.crawler()]()
	{
		auto pages = cl->waitForParsedPageReceived(StorageType::EmptyMetaDescriptionUrlStorageType, 2, 10, "Waiting for 2 empty meta description pages");
		cl->waitForCrawlingDone();
		cl->checkSequencedDataCollectionConsistency();
		EXPECT_EQ(2, pages.size());
		EXPECT_EQ(QString(), pages[0]->metaDescription);
		EXPECT_EQ(QString(), pages[1]->metaDescription);
		cl->waitForParsedPageReceived(StorageType::CrawledUrlStorageType, 2, 10, "Waiting for 2 crawled pages");
	};

	env.initializeTest(testFunction);
	env.exec();
}


TEST(MetaTagTests, NoMetaDescription)
{
	// no-meta.html -> no-meta-2.html
	TestEnvironment env;
	env.crawler()->options()->setData(TestEnvironment::defaultOptions({ Url("http://meta-desc.com/no-meta.html") }));

	const auto testFunction = [cl = env.crawler()]()
	{
		auto pages = cl->waitForParsedPageReceived(StorageType::EmptyMetaDescriptionUrlStorageType, 2, 10, "Waiting for 2 empty meta description pages");
		cl->waitForCrawlingDone();
		cl->checkSequencedDataCollectionConsistency();
		EXPECT_EQ(2, pages.size());
		EXPECT_EQ(QString::null, pages[0]->metaDescription);
		EXPECT_EQ(QString::null, pages[1]->metaDescription);
		cl->waitForParsedPageReceived(StorageType::CrawledUrlStorageType, 2, 10, "Waiting for 2 crawled pages");
	};

	env.initializeTest(testFunction);
	env.exec();
}


TEST(MetaTagTests, TooLongMetaDescription)
{
	// too-long-meta.html -> too-long-meta-2.html
	TestEnvironment env;

	auto options = TestEnvironment::defaultOptions(Url("http://meta-desc.com/too-long-meta.html"));
	options.maxDescriptionLength = 10;
	
	env.crawler()->options()->setData(options);

	const auto testFunction = [cl = env.crawler()]()
	{
		auto pages = cl->waitForParsedPageReceived(StorageType::TooLongMetaDescriptionUrlStorageType, 2, 10, "Waiting for 2 too long meta desc pages");
		cl->waitForCrawlingDone();
		cl->checkSequencedDataCollectionConsistency();
		EXPECT_EQ(2, pages.size());
		EXPECT_EQ(QString("This is too long meta description"), pages[0]->metaDescription);
		EXPECT_EQ(QString("This is too long meta description"), pages[1]->metaDescription);
		cl->waitForParsedPageReceived(StorageType::CrawledUrlStorageType, 2, 10, "Waiting for 2 crawled pages");
	};

	env.initializeTest(testFunction);
	env.exec();
}

TEST(MetaTagTests, TooShortMetaDescription)
{
	// too-short-meta.html -> too-short-meta-2.html
	TestEnvironment env;

	auto options = TestEnvironment::defaultOptions(Url("http://meta-desc.com/too-short-meta.html"));
	options.minDescriptionLength = 50;
	options.maxDescriptionLength = 100;

	env.crawler()->options()->setData(options);

	const auto testFunction = [cl = env.crawler()]()
	{
		auto pages = cl->waitForParsedPageReceived(StorageType::TooShortMetaDescriptionUrlStorageType, 2, 10, "Waiting for 2 too short meta desc pages");
		cl->waitForCrawlingDone();
		cl->checkSequencedDataCollectionConsistency();
		EXPECT_EQ(2, pages.size());
		EXPECT_EQ(QString("This is too short meta description"), pages[0]->metaDescription);
		EXPECT_EQ(QString("This is too short meta description"), pages[1]->metaDescription);
		cl->waitForParsedPageReceived(StorageType::CrawledUrlStorageType, 2, 10, "Waiting for 2 crawled pages");
	};

	env.initializeTest(testFunction);
	env.exec();
}

TEST(MetaTagTests, DuplicatedMetaDescriptions)
{
	// duplicated-meta-1.html -> duplicated-meta-2.html
	TestEnvironment env;
	env.crawler()->options()->setData(TestEnvironment::defaultOptions(Url("http://meta-desc.com/duplicated-meta-1.html")));

	const auto testFunction = [cl = env.crawler()]()
	{
		auto pages = cl->waitForParsedPageReceived(StorageType::DuplicatedMetaDescriptionUrlStorageType, 2, 10, "Waiting for 2 duplicated meta desc pages");
		cl->waitForCrawlingDone();
		cl->checkSequencedDataCollectionConsistency();
		EXPECT_EQ(2, pages.size());
		EXPECT_EQ(QString("Duplicated Meta Description"), pages[0]->metaDescription);
		EXPECT_EQ(QString("Duplicated Meta Description"), pages[1]->metaDescription);
		cl->waitForParsedPageReceived(StorageType::CrawledUrlStorageType, 2, 10, "Waiting for 2 crawled pages");
	};

	env.initializeTest(testFunction);
	env.exec();
}

TEST(MetaTagTests, DoNotIncludeCanonicalDuplicatedTitles)
{
	// canonical-duplicated-desc.html -> canonical-duplicated-desc.html

	TestEnvironment env;
	env.crawler()->options()->setData(TestEnvironment::defaultOptions(Url("http://meta-desc.com/canonical-duplicated-desc.html")));

	const auto testFunction = [cl = env.crawler()]()
	{
		cl->waitForAllCrawledPageReceived(10);
		auto pages = cl->storageItems(StorageType::DuplicatedMetaDescriptionUrlStorageType);
		EXPECT_EQ(0, pages.size());
	};

	env.initializeTest(testFunction);
	env.exec();
}

TEST(MetaTagTests, IncludeDuplicatedTitlesIfThereAreSeveralCanonical)
{
	// canonical-duplicated-desc-another.html -> canonical-desc-title.html -> canonical-duplicated-desc.html

	TestEnvironment env;
	env.crawler()->options()->setData(TestEnvironment::defaultOptions(Url("http://meta-desc.com/canonical-duplicated-desc-another.html")));

	const auto testFunction = [cl = env.crawler()]()
	{
		cl->waitForAllCrawledPageReceived(10);
		auto pages = cl->storageItems(StorageType::DuplicatedMetaDescriptionUrlStorageType);
		EXPECT_EQ(3, pages.size());
	};

	env.initializeTest(testFunction);
	env.exec();
}

TEST(MetaTagTests, DoNotIncludeEveryPageInDuplicatesMetaDescription)
{
	TestEnvironment env;
	env.crawler()->options()->setData(TestEnvironment::defaultOptions(Url("http://meta-desc.com/single-page.html")));

	const auto testFunction = [cl = env.crawler()]()
	{
		cl->waitForAllCrawledPageReceived(10);
		auto pages = cl->storageItems(StorageType::DuplicatedMetaDescriptionUrlStorageType);
		EXPECT_EQ(0, pages.size());
	};

	env.initializeTest(testFunction);
	env.exec();
}

TEST(MetaTagTests, SeveralMetaDescriptions)
{
	// several-titles.html -> several-titles-2.html

	TestEnvironment env;
	env.crawler()->options()->setData(TestEnvironment::defaultOptions(Url("http://meta-desc.com/several-meta.html")));

	const auto testFunction = [cl = env.crawler()]()
	{
		auto pages = cl->waitForParsedPageReceived(StorageType::SeveralMetaDescriptionUrlStorageType, 2, 10, "Waiting for 2 several meta desc pages");
		cl->waitForCrawlingDone();
		cl->checkSequencedDataCollectionConsistency();
		EXPECT_EQ(2, pages.size());
		EXPECT_EQ(QString("First Meta Description"), pages[0]->metaDescription);
		EXPECT_EQ(QString("First Meta Description"), pages[1]->metaDescription);
		cl->waitForParsedPageReceived(StorageType::CrawledUrlStorageType, 2, 10, "Waiting for 2 crawled pages");
	};

	env.initializeTest(testFunction);
	env.exec();
}

TEST(MetaTagTests, EmptyMetaKeywords)
{
	// empty-meta.html -> empty-meta-2.html

	TestEnvironment env;
	env.crawler()->options()->setData(TestEnvironment::defaultOptions({ Url("http://meta-keywords.com/empty-meta.html") }));

	const auto testFunction = [cl = env.crawler()]()
	{
		auto pages = cl->waitForParsedPageReceived(StorageType::EmptyMetaKeywordsUrlStorageType, 2, 10, "Waiting for 2 empty meta keywords pages");
		cl->waitForCrawlingDone();
		cl->checkSequencedDataCollectionConsistency();
		EXPECT_EQ(2, pages.size());
		EXPECT_EQ(QString(), pages[0]->metaKeywords);
		EXPECT_EQ(QString(), pages[1]->metaKeywords);
		cl->waitForParsedPageReceived(StorageType::CrawledUrlStorageType, 2, 10, "Waiting for 2 crawled pages");
	};

	env.initializeTest(testFunction);
	env.exec();
}


TEST(MetaTagTests, NoMetaKeywords)
{
	// no-meta.html -> no-meta-2.html

	TestEnvironment env;
	env.crawler()->options()->setData(TestEnvironment::defaultOptions({ Url("http://meta-keywords.com/no-meta.html") }));

	const auto testFunction = [cl = env.crawler()]()
	{
		auto pages = cl->waitForParsedPageReceived(StorageType::EmptyMetaKeywordsUrlStorageType, 2, 10, "Waiting for 2 empty meta keywords pages");
		cl->waitForCrawlingDone();
		cl->checkSequencedDataCollectionConsistency();
		EXPECT_EQ(2, pages.size());
		EXPECT_EQ(QString::null, pages[0]->metaKeywords);
		EXPECT_EQ(QString::null, pages[1]->metaKeywords);
		cl->waitForParsedPageReceived(StorageType::CrawledUrlStorageType, 2, 10, "Waiting for 2 crawled pages");
	};

	env.initializeTest(testFunction);
	env.exec();
}

TEST(MetaTagTests, DuplicatedMetaKeywords)
{
	// duplicated-meta-1.html -> duplicated-meta-2.html

	TestEnvironment env;
	env.crawler()->options()->setData(TestEnvironment::defaultOptions(Url("http://meta-keywords.com/duplicated-meta-1.html")));

	const auto testFunction = [cl = env.crawler()]()
	{
		auto pages = cl->waitForParsedPageReceived(StorageType::DuplicatedMetaKeywordsUrlStorageType, 2, 10, "Waiting for 2 duplicated meta keywords pages");
		cl->waitForCrawlingDone();
		cl->checkSequencedDataCollectionConsistency();
		EXPECT_EQ(2, pages.size());
		EXPECT_EQ(QString("keyword1 keyword2"), pages[0]->metaKeywords);
		EXPECT_EQ(QString("keyword1 keyword2"), pages[1]->metaKeywords);
		cl->waitForParsedPageReceived(StorageType::CrawledUrlStorageType, 2, 10, "Waiting for 2 crawled pages");
	};

	env.initializeTest(testFunction);
	env.exec();
}

TEST(MetaTagTests, DoNotIncludeCanonicalDuplicatedKeywords)
{
	// canonical-duplicated-key.html -> canonical-duplicated-key.html

	TestEnvironment env;
	env.crawler()->options()->setData(TestEnvironment::defaultOptions(Url("http://meta-keywords.com/canonical-duplicated-key.html")));

	const auto testFunction = [cl = env.crawler()]()
	{
		cl->waitForAllCrawledPageReceived(10);
		auto pages = cl->storageItems(StorageType::DuplicatedMetaKeywordsUrlStorageType);
		EXPECT_EQ(0, pages.size());
	};

	env.initializeTest(testFunction);
	env.exec();
}

TEST(MetaTagTests, IncludeDuplicatedKeywordsIfThereAreSeveralCanonical)
{
	// canonical-duplicated-key-another.html -> canonical-desc-key.html -> canonical-duplicated-key.html

	TestEnvironment env;
	env.crawler()->options()->setData(TestEnvironment::defaultOptions(Url("http://meta-keywords.com/canonical-duplicated-key-another.html")));

	const auto testFunction = [cl = env.crawler()]()
	{
		cl->waitForAllCrawledPageReceived(10);
		auto pages = cl->storageItems(StorageType::DuplicatedMetaKeywordsUrlStorageType);
		EXPECT_EQ(3, pages.size());
	};

	env.initializeTest(testFunction);
	env.exec();
}

TEST(MetaTagTests, DoNotIncludeEveryPageInDuplicatesMetaKeywords)
{
	TestEnvironment env;
	env.crawler()->options()->setData(TestEnvironment::defaultOptions(Url("http://meta-keywords.com/single-page.html")));

	const auto testFunction = [cl = env.crawler()]()
	{
		cl->waitForAllCrawledPageReceived(10);
		auto pages = cl->storageItems(StorageType::DuplicatedMetaKeywordsUrlStorageType);
		EXPECT_EQ(0, pages.size());
	};

	env.initializeTest(testFunction);
	env.exec();
}

TEST(MetaTagTests, SeveralMetaKeywords)
{
	// several-titles.html -> several-titles-2.html

	TestEnvironment env;
	env.crawler()->options()->setData(TestEnvironment::defaultOptions(Url("http://meta-keywords.com/several-meta.html")));

	const auto testFunction = [cl = env.crawler()]()
	{
		auto pages = cl->waitForParsedPageReceived(StorageType::SeveralMetaKeywordsUrlStorageType, 2, 10, "Waiting for 2 several meta keywords pages");
		cl->waitForCrawlingDone();
		cl->checkSequencedDataCollectionConsistency();
		EXPECT_EQ(2, pages.size());
		EXPECT_EQ(QString("keyword1 keyword2"), pages[0]->metaKeywords);
		EXPECT_EQ(QString("keyword1 keyword2"), pages[1]->metaKeywords);
		cl->waitForParsedPageReceived(StorageType::CrawledUrlStorageType, 2, 10, "Waiting for 2 crawled pages");
	};

	env.initializeTest(testFunction);
	env.exec();
}

TEST(MetaTagTests, MetaRefreshTagDetectionTest)
{
	TestEnvironment env;
	env.crawler()->options()->setData(TestEnvironment::defaultOptions(Url("http://metarefreshtag.com/index.html")));

	const auto testFunction = [cl = env.crawler()]()
	{
		auto pages = cl->waitForParsedPageReceived(StorageType::CrawledUrlStorageType, 2, 10, "Waiting for 2 pages");
		auto metaRefreshPages = cl->waitForParsedPageReceived(StorageType::ContainsMetaRefreshTagStorageType, 1, 10, "Waiting for 1 page");
		cl->waitForCrawlingDone();

		cl->checkSequencedDataCollectionConsistency();

		EXPECT_EQ(2, pages.size());
		EXPECT_EQ(1, metaRefreshPages.size());
	};

	env.initializeTest(testFunction);
	env.exec();
}

}