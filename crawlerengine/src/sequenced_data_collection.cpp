#include "sequenced_data_collection.h"

namespace CrawlerEngine
{

SequencedDataCollection::SequencedDataCollection()
{
	initializeStorages();
}

SequencedDataCollection::SequencedStorageType* SequencedDataCollection::storage(StorageType type) noexcept
{
	SequencedStorageType* storage = const_cast<SequencedStorageType*>(static_cast<const SequencedDataCollection* const>(this)->storage(type));
	return storage;
}

const SequencedDataCollection::SequencedStorageType* SequencedDataCollection::storage(StorageType type) const noexcept
{
	ASSERT(m_sequencedStorageMap.find(type) != m_sequencedStorageMap.end());

	const auto&[storageType, storage] = *m_sequencedStorageMap.find(type);

	Q_UNUSED(storageType);

	return storage.get();
}

void SequencedDataCollection::addParsedPage(ParsedPagePtr parsedPagePtr, int type)
{
	StorageType storageType = static_cast<StorageType>(type);

	auto storageIterator = m_sequencedStorageMap.find(storageType);

	if (storageIterator != m_sequencedStorageMap.end())
	{
		storageIterator->second->push_back(parsedPagePtr);

		emit parsedPageAdded(storage(storageType)->size() - 1, static_cast<int>(storageType));
	}
}

void SequencedDataCollection::onDataCleared()
{
	emit beginClearData();

	for (const auto& [storageType, collection] : m_sequencedStorageMap)
	{
		Q_UNUSED(storageType);

		collection->clear();
	}

	emit endClearData();
}

void SequencedDataCollection::initializeStorages()
{
	m_sequencedStorageMap = std::initializer_list<std::pair<const int, SequencedStorageTypePtr>>
	{
		std::make_pair(StorageType::CrawledUrlStorageType, std::make_shared<SequencedStorageType>()),
		std::make_pair(StorageType::ExternalUrlStorageType, std::make_shared<SequencedStorageType>()),

		std::make_pair(StorageType::UpperCaseUrlStorageType, std::make_shared<SequencedStorageType>()),
		std::make_pair(StorageType::NonAsciiCharacterUrlStorageType, std::make_shared<SequencedStorageType>()),
		std::make_pair(StorageType::VeryLongUrlStorageType, std::make_shared<SequencedStorageType>()),

		std::make_pair(StorageType::EmptyTitleUrlStorageType, std::make_shared<SequencedStorageType>()),
		std::make_pair(StorageType::DuplicatedTitleUrlStorageType, std::make_shared<SequencedStorageType>()),
		std::make_pair(StorageType::VeryLongTitleUrlStorageType, std::make_shared<SequencedStorageType>()),
		std::make_pair(StorageType::VeryShortTitleUrlStorageType, std::make_shared<SequencedStorageType>()),
		std::make_pair(StorageType::DuplicatedH1TitleUrlStorageType, std::make_shared<SequencedStorageType>()),
		std::make_pair(StorageType::SeveralTitleUrlStorageType, std::make_shared<SequencedStorageType>()),

		std::make_pair(StorageType::EmptyMetaDescriptionUrlStorageType, std::make_shared<SequencedStorageType>()),
		std::make_pair(StorageType::DuplicatedMetaDescriptionUrlStorageType, std::make_shared<SequencedStorageType>()),
		std::make_pair(StorageType::VeryLongMetaDescriptionUrlStorageType, std::make_shared<SequencedStorageType>()),
		std::make_pair(StorageType::VeryShortMetaDescriptionUrlStorageType, std::make_shared<SequencedStorageType>()),
		std::make_pair(StorageType::SeveralMetaDescriptionUrlStorageType, std::make_shared<SequencedStorageType>()),

		std::make_pair(StorageType::EmptyMetaKeywordsUrlStorageType, std::make_shared<SequencedStorageType>()),
		std::make_pair(StorageType::DuplicatedMetaKeywordsUrlStorageType, std::make_shared<SequencedStorageType>()),
		std::make_pair(StorageType::SeveralMetaKeywordsUrlStorageType, std::make_shared<SequencedStorageType>()),

		std::make_pair(StorageType::MissingH1UrlStorageType, std::make_shared<SequencedStorageType>()),
		std::make_pair(StorageType::DuplicatedH1UrlStorageType, std::make_shared<SequencedStorageType>()),
		std::make_pair(StorageType::VeryLongH1UrlStorageType, std::make_shared<SequencedStorageType>()),
		std::make_pair(StorageType::SeveralH1UrlStorageType, std::make_shared<SequencedStorageType>()),

		std::make_pair(StorageType::MissingH2UrlStorageType, std::make_shared<SequencedStorageType>()),
		std::make_pair(StorageType::DuplicatedH2UrlStorageType, std::make_shared<SequencedStorageType>()),
		std::make_pair(StorageType::VeryLongH2UrlStorageType, std::make_shared<SequencedStorageType>()),
		std::make_pair(StorageType::SeveralH2UrlStorageType, std::make_shared<SequencedStorageType>()),

		std::make_pair(StorageType::Over100kbImageStorageType, std::make_shared<SequencedStorageType>()),
		std::make_pair(StorageType::MissingAltTextImageStorageType, std::make_shared<SequencedStorageType>()),
		std::make_pair(StorageType::VeryLongAltTextImageStorageType, std::make_shared<SequencedStorageType>()),
		std::make_pair(StorageType::Status404StorageType, std::make_shared<SequencedStorageType>()),

		std::make_pair(StorageType::HtmlResourcesStorageType, std::make_shared<SequencedStorageType>()),
		std::make_pair(StorageType::ImageResourcesStorageType, std::make_shared<SequencedStorageType>()),
		std::make_pair(StorageType::JavaScriptResourcesStorageType, std::make_shared<SequencedStorageType>()),
		std::make_pair(StorageType::StyleSheetResourcesStorageType, std::make_shared<SequencedStorageType>()),
		std::make_pair(StorageType::FlashResourcesStorageType, std::make_shared<SequencedStorageType>()),
		std::make_pair(StorageType::VideoResourcesStorageType, std::make_shared<SequencedStorageType>()),
		std::make_pair(StorageType::OtherResourcesStorageType, std::make_shared<SequencedStorageType>()),

		std::make_pair(StorageType::ExternalHtmlResourcesStorageType, std::make_shared<SequencedStorageType>()),
		std::make_pair(StorageType::ExternalImageResourcesStorageType, std::make_shared<SequencedStorageType>()),
		std::make_pair(StorageType::ExternalJavaScriptResourcesStorageType, std::make_shared<SequencedStorageType>()),
		std::make_pair(StorageType::ExternalStyleSheetResourcesStorageType, std::make_shared<SequencedStorageType>()),
		std::make_pair(StorageType::ExternalFlashResourcesStorageType, std::make_shared<SequencedStorageType>()),
		std::make_pair(StorageType::ExternalVideoResourcesStorageType, std::make_shared<SequencedStorageType>()),
		std::make_pair(StorageType::ExternalOtherResourcesStorageType, std::make_shared<SequencedStorageType>())
	};
}

}