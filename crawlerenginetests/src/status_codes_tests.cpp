#include "stdafx.h"
namespace CrawlerEngineTests
{

using namespace CrawlerEngine;

TEST(StatusCodesTests, PagesWithNonSuccessfulCodesInSeoAnalysis)
{
	TestEnvironment env;

	auto options = TestEnvironment::defaultOptions(Url("http://status.com/too-short.html"));
	options.maxTitleLength = 10;
	options.minTitleLength = 9;

	options.maxDescriptionLength = 10;
	options.minDescriptionLength = 9;

	options.maxH1LengthChars = 10;
	options.maxH2LengthChars = 10;

	env.crawler()->options()->setData(options);

	const auto testFunction = [cl = env.crawler()]()
	{
		auto pages = cl->waitForParsedPageReceived(StorageType::CrawledUrlStorageType, 6, 10, "Waiting for 6 crawled pages");

		EXPECT_EQ(0, cl->storageItems(StorageType::EmptyTitleUrlStorageType).size());
		EXPECT_EQ(0, cl->storageItems(StorageType::DuplicatedTitleUrlStorageType).size());
		EXPECT_EQ(0, cl->storageItems(StorageType::TooLongTitleUrlStorageType).size());
		EXPECT_EQ(0, cl->storageItems(StorageType::TooShortTitleUrlStorageType).size());
		EXPECT_EQ(0, cl->storageItems(StorageType::DuplicatedH1TitleUrlStorageType).size());
		EXPECT_EQ(0, cl->storageItems(StorageType::SeveralTitleUrlStorageType).size());

		EXPECT_EQ(0, cl->storageItems(StorageType::EmptyMetaDescriptionUrlStorageType).size());
		EXPECT_EQ(0, cl->storageItems(StorageType::DuplicatedMetaDescriptionUrlStorageType).size());
		EXPECT_EQ(0, cl->storageItems(StorageType::TooLongMetaDescriptionUrlStorageType).size());
		EXPECT_EQ(0, cl->storageItems(StorageType::TooShortMetaDescriptionUrlStorageType).size());
		EXPECT_EQ(0, cl->storageItems(StorageType::SeveralMetaDescriptionUrlStorageType).size());

		EXPECT_EQ(0, cl->storageItems(StorageType::EmptyMetaKeywordsUrlStorageType).size());
		EXPECT_EQ(0, cl->storageItems(StorageType::DuplicatedMetaKeywordsUrlStorageType).size());
		EXPECT_EQ(0, cl->storageItems(StorageType::SeveralMetaKeywordsUrlStorageType).size());

		EXPECT_EQ(0, cl->storageItems(StorageType::MissingH1UrlStorageType).size());
		EXPECT_EQ(0, cl->storageItems(StorageType::DuplicatedH1UrlStorageType).size());
		EXPECT_EQ(0, cl->storageItems(StorageType::TooLongH1UrlStorageType).size());
		EXPECT_EQ(0, cl->storageItems(StorageType::SeveralH1UrlStorageType).size());

		EXPECT_EQ(0, cl->storageItems(StorageType::MissingH2UrlStorageType).size());
		EXPECT_EQ(0, cl->storageItems(StorageType::DuplicatedH2UrlStorageType).size());
		EXPECT_EQ(0, cl->storageItems(StorageType::TooLongH2UrlStorageType).size());
		EXPECT_EQ(0, cl->storageItems(StorageType::SeveralH2UrlStorageType).size());
	};

	env.initializeTest(testFunction);
	env.exec();
}

}