#include "sequenced_data_collection.h"
#include "sequenced_storage.h"

namespace CrawlerEngine
{

SequencedDataCollection::SequencedDataCollection()
	: m_crawledStorageSize(0)
{
}

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


size_t SequencedDataCollection::crawledStorageSize() const
{
	return m_crawledStorageSize;
}

std::shared_ptr<CrawlerEngine::ISequencedStorage> SequencedDataCollection::createSequencedStorage() const
{
	return std::static_pointer_cast<ISequencedStorage>(std::make_shared<SequencedStorage>());
}

void SequencedDataCollection::addParsedPage(ParsedPagePtr parsedPagePtr, int type)
{
	StorageType storageType = static_cast<StorageType>(type);

	auto storageIterator = m_sequencedStorageMap.find(storageType);

	if (type == StorageType::CrawledUrlStorageType)
	{
		++m_crawledStorageSize;
	}


	if (storageIterator != m_sequencedStorageMap.end())
	{
		auto&[storageType, collection] = *storageIterator;

		collection->emplaceBack(std::move(parsedPagePtr));

		emit parsedPageAdded(collection->size() - 1, static_cast<int>(storageType));
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

	m_crawledStorageSize.store(0);

	emit endClearData();
}

void SequencedDataCollection::initializeStorages()
{
	m_sequencedStorageMap = std::initializer_list<std::pair<const int, std::shared_ptr<ISequencedStorage>>>
	{
		std::make_pair(StorageType::CrawledUrlStorageType, createSequencedStorage()),
		std::make_pair(StorageType::ExternalUrlStorageType, createSequencedStorage()),

		std::make_pair(StorageType::UpperCaseUrlStorageType, createSequencedStorage()),
		std::make_pair(StorageType::NonAsciiCharacterUrlStorageType, createSequencedStorage()),
		std::make_pair(StorageType::VeryLongUrlStorageType, createSequencedStorage()),

		std::make_pair(StorageType::EmptyTitleUrlStorageType, createSequencedStorage()),
		std::make_pair(StorageType::DuplicatedTitleUrlStorageType, createSequencedStorage()),
		std::make_pair(StorageType::VeryLongTitleUrlStorageType, createSequencedStorage()),
		std::make_pair(StorageType::VeryShortTitleUrlStorageType, createSequencedStorage()),
		std::make_pair(StorageType::DuplicatedH1TitleUrlStorageType, createSequencedStorage()),
		std::make_pair(StorageType::SeveralTitleUrlStorageType, createSequencedStorage()),

		std::make_pair(StorageType::EmptyMetaDescriptionUrlStorageType, createSequencedStorage()),
		std::make_pair(StorageType::DuplicatedMetaDescriptionUrlStorageType, createSequencedStorage()),
		std::make_pair(StorageType::VeryLongMetaDescriptionUrlStorageType, createSequencedStorage()),
		std::make_pair(StorageType::VeryShortMetaDescriptionUrlStorageType, createSequencedStorage()),
		std::make_pair(StorageType::SeveralMetaDescriptionUrlStorageType, createSequencedStorage()),

		std::make_pair(StorageType::EmptyMetaKeywordsUrlStorageType, createSequencedStorage()),
		std::make_pair(StorageType::DuplicatedMetaKeywordsUrlStorageType, createSequencedStorage()),
		std::make_pair(StorageType::SeveralMetaKeywordsUrlStorageType, createSequencedStorage()),

		std::make_pair(StorageType::MissingH1UrlStorageType, createSequencedStorage()),
		std::make_pair(StorageType::DuplicatedH1UrlStorageType, createSequencedStorage()),
		std::make_pair(StorageType::VeryLongH1UrlStorageType, createSequencedStorage()),
		std::make_pair(StorageType::SeveralH1UrlStorageType, createSequencedStorage()),

		std::make_pair(StorageType::MissingH2UrlStorageType, createSequencedStorage()),
		std::make_pair(StorageType::DuplicatedH2UrlStorageType, createSequencedStorage()),
		std::make_pair(StorageType::VeryLongH2UrlStorageType, createSequencedStorage()),
		std::make_pair(StorageType::SeveralH2UrlStorageType, createSequencedStorage()),

		std::make_pair(StorageType::Over100kbImageStorageType, createSequencedStorage()),
		std::make_pair(StorageType::MissingAltTextImageStorageType, createSequencedStorage()),
		std::make_pair(StorageType::VeryLongAltTextImageStorageType, createSequencedStorage()),
		std::make_pair(StorageType::Status404StorageType, createSequencedStorage()),

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
		std::make_pair(StorageType::ExternalOtherResourcesStorageType, createSequencedStorage())
	};
}

}