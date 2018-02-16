#include "url.h"
#include "parsed_page.h"

namespace CrawlerEngineTests
{

using namespace CrawlerEngine;

TEST(RefreshTests, OldDuplicatesMustBeRemovedAfterRefresh)
{
	TestEnvironment env(TestEnvironment::defaultOptions({ Url("http://refresh1.com/index.html") }));

	const auto testFunction = [cl = env.crawler()]()
	{
		auto pages = cl->waitForAllCrawledPageReceived(10);
		cl->checkSequencedDataCollectionConsistency();

		const auto titleDuplicates = cl->storageItems(StorageType::DuplicatedTitleUrlStorageType);
		const auto metaDescriptionDuplicates = cl->storageItems(StorageType::DuplicatedMetaDescriptionUrlStorageType);

		EXPECT_EQ(2, titleDuplicates.size());
		EXPECT_EQ(2, metaDescriptionDuplicates.size());

		const auto firstTimeDuplicateTitle = [](const ParsedPage* page)
		{
			return page->title == QString("Dummy Title");
		};

		const auto firstTimeDuplicateMetaDescription = [](const ParsedPage* page)
		{
			return page->metaDescription == QString("Dummy Description");
		};

		const auto secondTimeDuplicateTitle = [](const ParsedPage* page)
		{
			return page->title == QString("Dummy Title Fixed");
		};

		const auto secondTimeDuplicateMetaDescription = [](const ParsedPage* page)
		{
			return page->metaDescription == QString("Dummy Description Fixed");
		};

		EXPECT_EQ(true, std::all_of(titleDuplicates.begin(), titleDuplicates.end(), firstTimeDuplicateTitle));
		EXPECT_EQ(true, std::all_of(metaDescriptionDuplicates.begin(), metaDescriptionDuplicates.end(), firstTimeDuplicateMetaDescription));

		const auto onPageRefreshed = [&]
		{
			const auto titleDuplicatesAfterRefresh = cl->storageItems(StorageType::DuplicatedTitleUrlStorageType);
			const auto metaDescriptionDuplicatesAfterRefresh = cl->storageItems(StorageType::DuplicatedMetaDescriptionUrlStorageType);

			EXPECT_EQ(2, titleDuplicatesAfterRefresh.size());
			EXPECT_EQ(2, metaDescriptionDuplicatesAfterRefresh.size());

			EXPECT_EQ(true, std::all_of(titleDuplicatesAfterRefresh.begin(), titleDuplicatesAfterRefresh.end(), secondTimeDuplicateTitle));
			EXPECT_EQ(true, std::all_of(metaDescriptionDuplicatesAfterRefresh.begin(), metaDescriptionDuplicatesAfterRefresh.end(), secondTimeDuplicateMetaDescription));
		};

		const Url refreshUrl("http://refresh1.com/refresh_page.html");

		const auto refreshPageIterator = std::find_if(pages.begin(), pages.end(), 
			[&refreshUrl](const ParsedPage* page) { return page->url == refreshUrl; });

		ASSERT(refreshPageIterator != pages.end());

		const int refreshPageIndex = std::distance(pages.begin(), refreshPageIterator);

		cl->refreshPage(StorageType::CrawledUrlStorageType, refreshPageIndex);

		cl->waitForRefreshPageDone(10);

		//
		// This delay is not reliable solution for waiting 
		// until results is not received by sequenced data collection
		//
		std::this_thread::sleep_for(std::chrono::seconds(3));

		onPageRefreshed();
	};

	env.initializeTest(testFunction);
	env.exec();
}

TEST(RefreshTests, OldDuplicatesMustBeRemovedAfterRefreshDifferentCanonicals)
{
	TestEnvironment env(TestEnvironment::defaultOptions({ Url("http://refresh2.com/index.html") }));

	const auto testFunction = [cl = env.crawler()]()
	{
		auto pages = cl->waitForAllCrawledPageReceived(10);
		cl->checkSequencedDataCollectionConsistency();

		const auto titleDuplicates = cl->storageItems(StorageType::DuplicatedTitleUrlStorageType);
		const auto metaDescriptionDuplicates = cl->storageItems(StorageType::DuplicatedMetaDescriptionUrlStorageType);

		EXPECT_EQ(3, titleDuplicates.size());
		EXPECT_EQ(3, metaDescriptionDuplicates.size());

		const auto duplicateTitle = [](const ParsedPage* page)
		{
			return page->title == QString("Dummy Title");
		};

		const auto duplicateMetaDescription = [](const ParsedPage* page)
		{
			return page->metaDescription == QString("Dummy Description");
		};

		EXPECT_EQ(true, std::all_of(titleDuplicates.begin(), titleDuplicates.end(), duplicateTitle));
		EXPECT_EQ(true, std::all_of(metaDescriptionDuplicates.begin(), metaDescriptionDuplicates.end(), duplicateMetaDescription));

		const auto onPageRefreshed = [&]
		{
			const auto titleDuplicatesAfterRefresh = cl->storageItems(StorageType::DuplicatedTitleUrlStorageType);
			const auto metaDescriptionDuplicatesAfterRefresh = cl->storageItems(StorageType::DuplicatedMetaDescriptionUrlStorageType);

			EXPECT_EQ(2, titleDuplicatesAfterRefresh.size());
			EXPECT_EQ(2, metaDescriptionDuplicatesAfterRefresh.size());

			EXPECT_EQ(true, std::all_of(titleDuplicatesAfterRefresh.begin(), titleDuplicatesAfterRefresh.end(), duplicateTitle));
			EXPECT_EQ(true, std::all_of(metaDescriptionDuplicatesAfterRefresh.begin(), metaDescriptionDuplicatesAfterRefresh.end(), duplicateMetaDescription));
		};

		const Url refreshUrl("http://refresh2.com/refresh_page.html");

		const auto refreshPageIterator = std::find_if(pages.begin(), pages.end(),
			[&refreshUrl](const ParsedPage* page) { return page->url == refreshUrl; });

		ASSERT(refreshPageIterator != pages.end());

		const int refreshPageIndex = std::distance(pages.begin(), refreshPageIterator);

		cl->refreshPage(StorageType::CrawledUrlStorageType, refreshPageIndex);

		cl->waitForRefreshPageDone(10);

		//
		// This delay is not reliable solution for waiting 
		// until results is not received by sequenced data collection
		//
		std::this_thread::sleep_for(std::chrono::seconds(3));

		onPageRefreshed();
	};

	env.initializeTest(testFunction);
	env.exec();
}

TEST(RefreshTests, DuplicatesMustNotBePresentedAfterRefresh)
{
	TestEnvironment env(TestEnvironment::defaultOptions({ Url("http://refresh3.com/index.html") }));

	const auto testFunction = [cl = env.crawler()]()
	{
		auto pages = cl->waitForAllCrawledPageReceived(10);
		cl->checkSequencedDataCollectionConsistency();

		const auto titleDuplicates = cl->storageItems(StorageType::DuplicatedTitleUrlStorageType);
		const auto metaDescriptionDuplicates = cl->storageItems(StorageType::DuplicatedMetaDescriptionUrlStorageType);

		EXPECT_EQ(3, titleDuplicates.size());
		EXPECT_EQ(3, metaDescriptionDuplicates.size());

		const auto duplicateTitle = [](const ParsedPage* page)
		{
			return page->title == QString("Dummy Title");
		};

		const auto duplicateMetaDescription = [](const ParsedPage* page)
		{
			return page->metaDescription == QString("Dummy Description");
		};

		EXPECT_EQ(true, std::all_of(titleDuplicates.begin(), titleDuplicates.end(), duplicateTitle));
		EXPECT_EQ(true, std::all_of(metaDescriptionDuplicates.begin(), metaDescriptionDuplicates.end(), duplicateMetaDescription));

		const auto onPageRefreshed = [&]
		{
			const auto titleDuplicatesAfterRefresh = cl->storageItems(StorageType::DuplicatedTitleUrlStorageType);
			const auto metaDescriptionDuplicatesAfterRefresh = cl->storageItems(StorageType::DuplicatedMetaDescriptionUrlStorageType);

			EXPECT_EQ(0, titleDuplicatesAfterRefresh.size());
			EXPECT_EQ(0, metaDescriptionDuplicatesAfterRefresh.size());
		};

		const Url refreshUrl("http://refresh3.com/refresh_page.html");

		const auto refreshPageIterator = std::find_if(pages.begin(), pages.end(),
			[&refreshUrl](const ParsedPage* page) { return page->url == refreshUrl; });

		ASSERT(refreshPageIterator != pages.end());

		const int refreshPageIndex = std::distance(pages.begin(), refreshPageIterator);

		cl->refreshPage(StorageType::CrawledUrlStorageType, refreshPageIndex);

		cl->waitForRefreshPageDone(10);

		//
		// This delay is not reliable solution for waiting 
		// until results is not received by sequenced data collection
		//
		std::this_thread::sleep_for(std::chrono::seconds(3));

		onPageRefreshed();
	};

	env.initializeTest(testFunction);
	env.exec();
}


}