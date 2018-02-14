#include "url.h"
#include "parsed_page.h"

namespace CrawlerEngineTests
{

using namespace CrawlerEngine;

TEST(RefreshTests, DuplicatesMustBeRemovedAfterRefresh)
{
	TestEnvironment env(TestEnvironment::defaultOptions({ Url("http://refresh.com/index.html") }));

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
			EXPECT_EQ(2, titleDuplicates.size());
			EXPECT_EQ(2, metaDescriptionDuplicates.size());

			EXPECT_EQ(true, std::all_of(titleDuplicates.begin(), titleDuplicates.end(), secondTimeDuplicateTitle));
			EXPECT_EQ(true, std::all_of(metaDescriptionDuplicates.begin(), metaDescriptionDuplicates.end(), secondTimeDuplicateMetaDescription));
		};

		const Url refreshUrl("http://refresh.com/refresh_page.html");

		const auto refreshPageIterator = std::find_if(pages.begin(), pages.end(), 
			[&refreshUrl](const ParsedPage* page) { return page->url == refreshUrl; });

		ASSERT(refreshPageIterator != pages.end());

		const int refreshPageIndex = std::distance(pages.begin(), refreshPageIterator);

		cl->refreshPage(StorageType::CrawledUrlStorageType, refreshPageIndex);

		cl->waitForRefreshPageDone(10);

		onPageRefreshed();
	};

	env.initializeTest(testFunction);
	env.exec();
}

}