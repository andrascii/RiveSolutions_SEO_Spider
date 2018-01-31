#include "sequenced_data_collection.h"
#include "sequenced_storage.h"
#include "crawler_shared_state.h"

namespace CrawlerEngine
{

bool SequencedDataCollection::empty() const noexcept
{
	return storage(StorageType::CrawledUrlStorageType)->empty();
}

ISequencedStorage* SequencedDataCollection::storage(StorageType type) noexcept
{
	ISequencedStorage* storage = const_cast<ISequencedStorage*>(static_cast<const SequencedDataCollection* const>(this)->storage(type));
	return storage;
}

const ISequencedStorage* SequencedDataCollection::storage(StorageType type) const noexcept
{
	ASSERT(m_sequencedStorageMap.find(type) != m_sequencedStorageMap.end());

	auto iter = m_sequencedStorageMap.find(type);

	return iter != m_sequencedStorageMap.end() ? iter->second.get() : nullptr;
}

std::shared_ptr<CrawlerEngine::ISequencedStorage> SequencedDataCollection::createSequencedStorage() const
{
	return std::static_pointer_cast<ISequencedStorage>(std::make_shared<SequencedStorage>());
}

void SequencedDataCollection::addParsedPage(ParsedPagePtr parsedPagePtr, StorageType type)
{
	auto storageIterator = m_sequencedStorageMap.find(type);

	if (type == StorageType::CrawledUrlStorageType)
	{
		CrawlerSharedState::instance()->incrementSequencedDataCollectionLinksCount();
	}

	if (storageIterator != m_sequencedStorageMap.end())
	{
		auto&[storageType, collection] = *storageIterator;

		collection->emplaceBack(std::move(parsedPagePtr));

		emit parsedPageAdded(collection->size() - 1, storageType);
	}
}

void SequencedDataCollection::onDataCleared()
{
	emit beginClearData();

	for (auto& [storageType, collection] : m_sequencedStorageMap)
	{
		if (storageType == StorageType::CrawledUrlStorageType)
		{
			continue;
		}

		collection->clear();
	}

	std::shared_ptr<ISequencedStorage>& sequencedStorage = m_sequencedStorageMap[StorageType::CrawledUrlStorageType];

	ASSERT(!sequencedStorage->containsPointersWithUseCountGreaterThanOne());

	sequencedStorage->clear();

	CrawlerSharedState::instance()->setSequencedDataCollectionLinksCount(0);

	emit endClearData();
}

void SequencedDataCollection::initializeStorages()
{
	m_sequencedStorageMap = std::initializer_list<std::pair<const StorageType, std::shared_ptr<ISequencedStorage>>>
	{
		std::make_pair(StorageType::CrawledUrlStorageType, createSequencedStorage()),

		// Link Problems Storages
		std::make_pair(StorageType::ExternalUrlStorageType, createSequencedStorage()),
		std::make_pair(StorageType::UpperCaseUrlStorageType, createSequencedStorage()),
		std::make_pair(StorageType::NonAsciiCharacterUrlStorageType, createSequencedStorage()),
		std::make_pair(StorageType::TooLongUrlStorageType, createSequencedStorage()),
		std::make_pair(StorageType::BrokenLinks, createSequencedStorage()),
		std::make_pair(StorageType::WwwRedirectionsUrlStorageType, createSequencedStorage()),
		std::make_pair(StorageType::Status4xxStorageType, createSequencedStorage()),
		std::make_pair(StorageType::Status5xxStorageType, createSequencedStorage()),
		std::make_pair(StorageType::Status302StorageType, createSequencedStorage()),
		std::make_pair(StorageType::Status301StorageType, createSequencedStorage()),

		// Title Storages
		std::make_pair(StorageType::EmptyTitleUrlStorageType, createSequencedStorage()),
		std::make_pair(StorageType::DuplicatedTitleUrlStorageType, createSequencedStorage()),
		std::make_pair(StorageType::TooLongTitleUrlStorageType, createSequencedStorage()),
		std::make_pair(StorageType::TooShortTitleUrlStorageType, createSequencedStorage()),
		std::make_pair(StorageType::DuplicatedH1TitleUrlStorageType, createSequencedStorage()),
		std::make_pair(StorageType::SeveralTitleUrlStorageType, createSequencedStorage()),

		// Meta Description Storages
		std::make_pair(StorageType::EmptyMetaDescriptionUrlStorageType, createSequencedStorage()),
		std::make_pair(StorageType::DuplicatedMetaDescriptionUrlStorageType, createSequencedStorage()),
		std::make_pair(StorageType::TooLongMetaDescriptionUrlStorageType, createSequencedStorage()),
		std::make_pair(StorageType::TooShortMetaDescriptionUrlStorageType, createSequencedStorage()),
		std::make_pair(StorageType::SeveralMetaDescriptionUrlStorageType, createSequencedStorage()),

		// Meta Keywords Problems Storages
		std::make_pair(StorageType::EmptyMetaKeywordsUrlStorageType, createSequencedStorage()),
		std::make_pair(StorageType::DuplicatedMetaKeywordsUrlStorageType, createSequencedStorage()),
		std::make_pair(StorageType::SeveralMetaKeywordsUrlStorageType, createSequencedStorage()),
		
		// H1 Problems Storages
		std::make_pair(StorageType::MissingH1UrlStorageType, createSequencedStorage()),
		std::make_pair(StorageType::DuplicatedH1UrlStorageType, createSequencedStorage()),
		std::make_pair(StorageType::TooLongH1UrlStorageType, createSequencedStorage()),
		std::make_pair(StorageType::SeveralH1UrlStorageType, createSequencedStorage()),

		// H2 Problems Storages
		std::make_pair(StorageType::MissingH2UrlStorageType, createSequencedStorage()),
		std::make_pair(StorageType::DuplicatedH2UrlStorageType, createSequencedStorage()),
		std::make_pair(StorageType::TooLongH2UrlStorageType, createSequencedStorage()),
		std::make_pair(StorageType::SeveralH2UrlStorageType, createSequencedStorage()),

		// Images Problems Storages
		std::make_pair(StorageType::Over100kbImageStorageType, createSequencedStorage()),
		std::make_pair(StorageType::MissingAltTextImageStorageType, createSequencedStorage()),
		std::make_pair(StorageType::TooLongAltTextImageStorageType, createSequencedStorage()),

		// Page problems
		std::make_pair(StorageType::TooManyLinksOnPageStorageType, createSequencedStorage()),
		std::make_pair(StorageType::ContainsMetaRefreshTagStorageType, createSequencedStorage()),
		std::make_pair(StorageType::ContainsFramesStorageType, createSequencedStorage()),

		// Resources Types Storages
		std::make_pair(StorageType::HtmlResourcesStorageType, createSequencedStorage()),
		std::make_pair(StorageType::ImageResourcesStorageType, createSequencedStorage()),
		std::make_pair(StorageType::JavaScriptResourcesStorageType, createSequencedStorage()),
		std::make_pair(StorageType::StyleSheetResourcesStorageType, createSequencedStorage()),
		std::make_pair(StorageType::FlashResourcesStorageType, createSequencedStorage()),
		std::make_pair(StorageType::VideoResourcesStorageType, createSequencedStorage()),
		std::make_pair(StorageType::OtherResourcesStorageType, createSequencedStorage()),
		std::make_pair(StorageType::ExternalHtmlResourcesStorageType, createSequencedStorage()),
		std::make_pair(StorageType::ExternalImageResourcesStorageType, createSequencedStorage()),
		std::make_pair(StorageType::ExternalJavaScriptResourcesStorageType, createSequencedStorage()),
		std::make_pair(StorageType::ExternalStyleSheetResourcesStorageType, createSequencedStorage()),
		std::make_pair(StorageType::ExternalFlashResourcesStorageType, createSequencedStorage()),
		std::make_pair(StorageType::ExternalVideoResourcesStorageType, createSequencedStorage()),
		std::make_pair(StorageType::ExternalOtherResourcesStorageType, createSequencedStorage()),
		std::make_pair(StorageType::CanonicalUrlResourcesStorageType, createSequencedStorage()),
		std::make_pair(StorageType::UniqueCanonicalUrlResourcesStorageType, createSequencedStorage()),
		std::make_pair(StorageType::ExternalDoFollowUrlResourcesStorageType, createSequencedStorage()),
	};
}

}
