#include "error_category.h"
#include "crawler.h"
#include "sequenced_data_collection.h"
#include "isequenced_storage.h"

namespace CrawlerEngine
{

ErrorCategory::ErrorCategoryLevel ErrorCategory::level(StorageType type) noexcept
{
	DEBUG_ASSERT(type > StorageType::BeginEnumStorageType && type < StorageType::EndEnumStorageType);

	const SequencedDataCollection* sequencedDataCollection = Crawler::instance().sequencedDataCollection();
	const ISequencedStorage* storage = sequencedDataCollection->storage(type);
	const int storageSize = storage->size();

	switch (type)
	{
		// Url problems
		case StorageType::UpperCaseUrlStorageType:
		{
			return !storageSize ? LevelNotError : LevelWarning;
		}
		case StorageType::NonAsciiCharacterUrlStorageType:
		{
			return !storageSize ? LevelNotError : LevelWarning;
		}
		case StorageType::TooLongUrlStorageType:
		{
			return !storageSize ? LevelNotError : LevelInfo;
		}
		case StorageType::BrokenLinks:
		{
			return !storageSize ? LevelNotError : LevelError;
		}
		case StorageType::Status4xxStorageType:
		{
			return !storageSize ? LevelNotError : LevelError;
		}
		case StorageType::Status5xxStorageType:
		{
			return !storageSize ? LevelNotError : LevelError;
		}
		case StorageType::Status302StorageType:
		{
			return !storageSize ? LevelNotError : LevelError;
		}
		case StorageType::Status301StorageType:
		{
			return !storageSize ? LevelNotError : LevelInfo;
		}

		// Title problems
		case StorageType::EmptyTitleUrlStorageType:
		{
			return !storageSize ? LevelNotError : LevelWarning;
		}
		case StorageType::DuplicatedTitleUrlStorageType:
		{
			return !storageSize ? LevelNotError : LevelError;
		}
		case StorageType::TooLongTitleUrlStorageType:
		{
			return !storageSize ? LevelNotError : LevelWarning;
		}
		case StorageType::TooShortTitleUrlStorageType:
		{
			return !storageSize ? LevelNotError : LevelWarning;
		}
		case StorageType::DuplicatedH1TitleUrlStorageType:
		{
			return !storageSize ? LevelNotError : LevelError;
		}
		case StorageType::SeveralTitleUrlStorageType:
		{
			return !storageSize ? LevelNotError : LevelError;
		}

		// Meta description problems
		case StorageType::EmptyMetaDescriptionUrlStorageType:
		{
			return !storageSize ? LevelNotError : LevelWarning;
		}
		case StorageType::DuplicatedMetaDescriptionUrlStorageType:
		{
			return !storageSize ? LevelNotError : LevelError;
		}
		case StorageType::TooLongMetaDescriptionUrlStorageType:
		{
			return !storageSize ? LevelNotError : LevelWarning;
		}
		case StorageType::TooShortMetaDescriptionUrlStorageType:
		{
			return !storageSize ? LevelNotError : LevelWarning;
		}
		case StorageType::SeveralMetaDescriptionUrlStorageType:
		{
			return !storageSize ? LevelNotError : LevelError;
		}

		// Meta keywords problems
		case StorageType::EmptyMetaKeywordsUrlStorageType:
		{
			return !storageSize ? LevelNotError : LevelWarning;
		}
		case StorageType::DuplicatedMetaKeywordsUrlStorageType:
		{
			return !storageSize ? LevelNotError : LevelError;
		}
		case StorageType::SeveralMetaKeywordsUrlStorageType:
		{
			return !storageSize ? LevelNotError : LevelError;
		}

		// H1 problems
		case StorageType::MissingH1UrlStorageType:
		{
			return !storageSize ? LevelNotError : LevelInfo;
		}
		case StorageType::DuplicatedH1UrlStorageType:
		{
			return !storageSize ? LevelNotError : LevelError;
		}
		case StorageType::TooLongH1UrlStorageType:
		{
			return !storageSize ? LevelNotError : LevelInfo;
		}
		case StorageType::SeveralH1UrlStorageType:
		{
			return !storageSize ? LevelNotError : LevelWarning;
		}

		// H2 problems
		case StorageType::MissingH2UrlStorageType:
		{
			return !storageSize ? LevelNotError : LevelInfo;
		}
		case StorageType::DuplicatedH2UrlStorageType:
		{
			return !storageSize ? LevelNotError : LevelError;
		}
		case StorageType::TooLongH2UrlStorageType:
		{
			return !storageSize ? LevelNotError : LevelInfo;
		}
		case StorageType::SeveralH2UrlStorageType:
		{
			return !storageSize ? LevelNotError : LevelWarning;
		}

		// Images problems
		case StorageType::Over100kbImageStorageType:
		{
			return !storageSize ? LevelNotError : LevelInfo;
		}
		case StorageType::MissingAltTextImageStorageType:
		{
			return !storageSize ? LevelNotError : LevelInfo;
		}
		case StorageType::TooLongAltTextImageStorageType:
		{
			return !storageSize ? LevelNotError : LevelInfo;
		}

		// page problems
		case StorageType::TooManyLinksOnPageStorageType:
		{
			return !storageSize ? LevelNotError : LevelWarning;
		}
		case StorageType::ContainsMetaRefreshTagStorageType:
		{
			return !storageSize ? LevelNotError : LevelWarning;
		}
		case StorageType::ContainsFramesStorageType:
		{
			return !storageSize ? LevelNotError : LevelError;
		}
	}

	DEBUG_ASSERT(!"Not handled storage type");
	return ErrorCategoryLevel();
}

int ErrorCategory::infoCount() noexcept
{
	const SequencedDataCollection* sequencedDataCollection = Crawler::instance().sequencedDataCollection();

	const int infoCounter =
		sequencedDataCollection->storage(StorageType::TooLongUrlStorageType)->size() +
		sequencedDataCollection->storage(StorageType::Status301StorageType)->size() +
		sequencedDataCollection->storage(StorageType::MissingH1UrlStorageType)->size() +
		sequencedDataCollection->storage(StorageType::TooLongH1UrlStorageType)->size() +
		sequencedDataCollection->storage(StorageType::MissingH2UrlStorageType)->size() +
		sequencedDataCollection->storage(StorageType::TooLongH2UrlStorageType)->size() +
		sequencedDataCollection->storage(StorageType::Over100kbImageStorageType)->size() +
		sequencedDataCollection->storage(StorageType::MissingAltTextImageStorageType)->size() +
		sequencedDataCollection->storage(StorageType::TooLongAltTextImageStorageType)->size();

	return infoCounter;
}

int ErrorCategory::warningCount() noexcept
{
	const SequencedDataCollection* sequencedDataCollection = Crawler::instance().sequencedDataCollection();

	const int infoCounter =
		sequencedDataCollection->storage(StorageType::UpperCaseUrlStorageType)->size() +
		sequencedDataCollection->storage(StorageType::NonAsciiCharacterUrlStorageType)->size() +
		sequencedDataCollection->storage(StorageType::EmptyTitleUrlStorageType)->size() +
		sequencedDataCollection->storage(StorageType::TooLongTitleUrlStorageType)->size() +
		sequencedDataCollection->storage(StorageType::TooShortTitleUrlStorageType)->size() +
		sequencedDataCollection->storage(StorageType::EmptyMetaDescriptionUrlStorageType)->size() +
		sequencedDataCollection->storage(StorageType::TooLongMetaDescriptionUrlStorageType)->size() +
		sequencedDataCollection->storage(StorageType::TooShortMetaDescriptionUrlStorageType)->size() +
		sequencedDataCollection->storage(StorageType::EmptyMetaKeywordsUrlStorageType)->size() + 
		sequencedDataCollection->storage(StorageType::SeveralH1UrlStorageType)->size() +
		sequencedDataCollection->storage(StorageType::SeveralH2UrlStorageType)->size();

	return infoCounter;
}

int ErrorCategory::errorCount() noexcept
{
	const SequencedDataCollection* sequencedDataCollection = Crawler::instance().sequencedDataCollection();

	const int infoCounter =
		sequencedDataCollection->storage(StorageType::BrokenLinks)->size() +
		sequencedDataCollection->storage(StorageType::Status4xxStorageType)->size() +
		sequencedDataCollection->storage(StorageType::Status5xxStorageType)->size() +
		sequencedDataCollection->storage(StorageType::Status302StorageType)->size() +
		sequencedDataCollection->storage(StorageType::DuplicatedTitleUrlStorageType)->size() +
		sequencedDataCollection->storage(StorageType::DuplicatedH1TitleUrlStorageType)->size() +
		sequencedDataCollection->storage(StorageType::SeveralTitleUrlStorageType)->size() +
		sequencedDataCollection->storage(StorageType::DuplicatedMetaDescriptionUrlStorageType)->size() +
		sequencedDataCollection->storage(StorageType::SeveralMetaDescriptionUrlStorageType)->size() +
		sequencedDataCollection->storage(StorageType::DuplicatedMetaKeywordsUrlStorageType)->size() +
		sequencedDataCollection->storage(StorageType::SeveralMetaKeywordsUrlStorageType)->size() +
		sequencedDataCollection->storage(StorageType::DuplicatedH1UrlStorageType)->size() +
		sequencedDataCollection->storage(StorageType::ContainsFramesStorageType)->size() +
		sequencedDataCollection->storage(StorageType::DuplicatedH2UrlStorageType)->size();

	return infoCounter;
}

}