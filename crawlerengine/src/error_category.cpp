#include "stdafx.h"
#include "error_category.h"
#include "crawler.h"
#include "sequenced_data_collection.h"
#include "isequenced_storage.h"

namespace
{

using namespace CrawlerEngine;

const QMap<StorageType, ErrorCategory::ErrorCategoryLevel> s_storageTypeToLevel
{
	// links
	{ StorageType::UpperCaseUrlStorageType, ErrorCategory::LevelWarning },
	{ StorageType::NonAsciiCharacterUrlStorageType, ErrorCategory::LevelWarning },
	{ StorageType::TooLongUrlStorageType, ErrorCategory::LevelInfo },
	{ StorageType::BrokenLinks, ErrorCategory::LevelError },
	{ StorageType::WwwRedirectionsUrlStorageType, ErrorCategory::LevelWarning },
	{ StorageType::TooManyRedirectsStorageType, ErrorCategory::LevelWarning },
	{ StorageType::TimeoutStorageType, ErrorCategory::LevelWarning },
	{ StorageType::Status4xxStorageType, ErrorCategory::LevelError },
	{ StorageType::Status5xxStorageType, ErrorCategory::LevelError },
	{ StorageType::Status302StorageType, ErrorCategory::LevelError },
	{ StorageType::Status301StorageType, ErrorCategory::LevelWarning },
	{ StorageType::ExternalDoFollowUrlResourcesStorageType, ErrorCategory::LevelInfo },

	// titles
	{ StorageType::EmptyTitleUrlStorageType, ErrorCategory::LevelWarning },
	{ StorageType::DuplicatedTitleUrlStorageType, ErrorCategory::LevelError },
	{ StorageType::TooLongTitleUrlStorageType, ErrorCategory::LevelWarning },
	{ StorageType::TooShortTitleUrlStorageType, ErrorCategory::LevelWarning },
	{ StorageType::DuplicatedH1TitleUrlStorageType, ErrorCategory::LevelError },
	{ StorageType::SeveralTitleUrlStorageType, ErrorCategory::LevelError },

	// meta descriptions
	{ StorageType::EmptyMetaDescriptionUrlStorageType, ErrorCategory::LevelWarning },
	{ StorageType::DuplicatedMetaDescriptionUrlStorageType, ErrorCategory::LevelError },
	{ StorageType::TooLongMetaDescriptionUrlStorageType, ErrorCategory::LevelWarning },
	{ StorageType::TooShortMetaDescriptionUrlStorageType, ErrorCategory::LevelWarning },
	{ StorageType::SeveralMetaDescriptionUrlStorageType, ErrorCategory::LevelError },

	// meta keywords
	{ StorageType::EmptyMetaKeywordsUrlStorageType, ErrorCategory::LevelInfo },
	{ StorageType::DuplicatedMetaKeywordsUrlStorageType, ErrorCategory::LevelInfo },
	{ StorageType::SeveralMetaKeywordsUrlStorageType, ErrorCategory::LevelInfo },

	// h1s
	{ StorageType::MissingH1UrlStorageType, ErrorCategory::LevelInfo },
	{ StorageType::DuplicatedH1UrlStorageType, ErrorCategory::LevelError },
	{ StorageType::TooLongH1UrlStorageType, ErrorCategory::LevelInfo },
	{ StorageType::SeveralH1UrlStorageType, ErrorCategory::LevelWarning },

	// h2s
	{ StorageType::MissingH2UrlStorageType, ErrorCategory::LevelInfo },
	{ StorageType::DuplicatedH2UrlStorageType, ErrorCategory::LevelError },
	{ StorageType::TooLongH2UrlStorageType, ErrorCategory::LevelInfo },
	{ StorageType::SeveralH2UrlStorageType, ErrorCategory::LevelWarning },

	// images
	{ StorageType::TooBigImageStorageType, ErrorCategory::LevelInfo },
	{ StorageType::MissingAltTextImageStorageType, ErrorCategory::LevelInfo },
	{ StorageType::TooLongAltTextImageStorageType, ErrorCategory::LevelInfo },
	{ StorageType::BrokenImagesStorageType, ErrorCategory::LevelError },

	// page problems
	{ StorageType::TooManyLinksOnPageStorageType, ErrorCategory::LevelWarning },
	{ StorageType::ContainsMetaRefreshTagStorageType, ErrorCategory::LevelWarning },
	{ StorageType::ContainsFramesStorageType, ErrorCategory::LevelError },
	{ StorageType::TooBigHtmlResourcesStorageType, ErrorCategory::LevelWarning },
	{ StorageType::DuplicatedCanonicalUrlResourcesStorageType, ErrorCategory::LevelInfo },
	{ StorageType::AllCanonicalUrlResourcesStorageType, ErrorCategory::LevelInfo },

	// not indexed pages
	{ StorageType::BlockedForSEIndexingStorageType, ErrorCategory::LevelInfo },
	{ StorageType::NofollowLinksStorageType, ErrorCategory::LevelInfo },
	{ StorageType::BlockedByRobotsTxtStorageType, ErrorCategory::LevelInfo },
	{ StorageType::BlockedByXRobotsTagStorageType, ErrorCategory::LevelInfo },

	// yandex metrica counters
	{ StorageType::YandexMetricaCounter1StorageType, ErrorCategory::LevelError },
	{ StorageType::YandexMetricaCounter2StorageType, ErrorCategory::LevelError },
	{ StorageType::YandexMetricaCounter3StorageType, ErrorCategory::LevelError },
	{ StorageType::YandexMetricaCounter4StorageType, ErrorCategory::LevelError },
	{ StorageType::YandexMetricaCounter5StorageType, ErrorCategory::LevelError }
};

}

namespace CrawlerEngine
{

ErrorCategory::ErrorCategoryLevel ErrorCategory::level(StorageType type) noexcept
{
	DEBUG_ASSERT(type > StorageType::BeginEnumStorageType && type < StorageType::EndEnumStorageType);

	const SequencedDataCollection* sequencedDataCollection = Crawler::instance().sequencedDataCollection();
	const ISequencedStorage* storage = sequencedDataCollection->storage(type);
	const int storageSize = storage->size();

	return !storageSize ? LevelNotError : s_storageTypeToLevel[type];
}

int ErrorCategory::infoCount() noexcept
{
	const SequencedDataCollection* sequencedDataCollection = Crawler::instance().sequencedDataCollection();

	int count = 0;

	for (auto iterator = s_storageTypeToLevel.begin(); iterator != s_storageTypeToLevel.end(); ++iterator)
	{
		if (iterator.value() == LevelInfo)
		{
			count += sequencedDataCollection->storage(iterator.key())->size();
		}
	}

	return count;
}

int ErrorCategory::warningCount() noexcept
{
	const SequencedDataCollection* sequencedDataCollection = Crawler::instance().sequencedDataCollection();

	int count = 0;

	for (auto iterator = s_storageTypeToLevel.begin(); iterator != s_storageTypeToLevel.end(); ++iterator)
	{
		if (iterator.value() == LevelWarning)
		{
			count += sequencedDataCollection->storage(iterator.key())->size();
		}
	}

	return count;
}

int ErrorCategory::errorCount() noexcept
{
	const SequencedDataCollection* sequencedDataCollection = Crawler::instance().sequencedDataCollection();

	int count = 0;

	for (auto iterator = s_storageTypeToLevel.begin(); iterator != s_storageTypeToLevel.end(); ++iterator)
	{
		if (iterator.value() == LevelError)
		{
			count += sequencedDataCollection->storage(iterator.key())->size();
		}
	}

	return count;
}

}