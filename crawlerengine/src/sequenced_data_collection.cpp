#include "sequenced_data_collection.h"

namespace CrawlerEngine
{

int SequencedStorage::size() const noexcept
{
	return m_pages.size();
}

void SequencedStorage::clear()
{
	m_pages.clear();
}

void SequencedStorage::pushBack(const ParsedPagePtr& page)
{
	m_pages.push_back(page);
}

const ParsedPage* SequencedStorage::operator[](int idx) const noexcept
{
	ASSERT(idx >= 0 && idx < m_pages.size());

	return m_pages[idx].get();
}

ParsedPage* SequencedStorage::operator[](int idx) noexcept
{
	const SequencedStorage& thisConst = *static_cast<SequencedStorage const * const>(this);

	return const_cast<ParsedPage*>(thisConst[idx]);
}

bool SequencedStorage::containsPointersWithUseCountGreaterThanOne() const noexcept
{
	int pagesWithUseCountGreaterThanOne = 0;
	for (const ParsedPagePtr& pagePointer : m_pages)
	{
		if (pagePointer.use_count() > 1)
		{
			pagesWithUseCountGreaterThanOne++;
		}
	}

	if (pagesWithUseCountGreaterThanOne > 0)
	{
		ERRORLOG << "Some pages will not be destroyed, count: " << pagesWithUseCountGreaterThanOne;
		return true;
	}

	return false;
}

//////////////////////////////////////////////////////////////////////////

SequencedDataCollection::SequencedDataCollection()
{
	initializeStorages();
}

SequencedStorage* SequencedDataCollection::storage(StorageType type) noexcept
{
	SequencedStorage* storage = const_cast<SequencedStorage*>(static_cast<const SequencedDataCollection* const>(this)->storage(type));
	return storage;
}

const SequencedStorage* SequencedDataCollection::storage(StorageType type) const noexcept
{
	ASSERT(m_sequencedStorageMap.find(type) != m_sequencedStorageMap.end());

	const auto&[storageType, storage] = *m_sequencedStorageMap.find(type);

	Q_UNUSED(storageType);

	return &storage;
}

void SequencedDataCollection::addParsedPage(ParsedPagePtr parsedPagePtr, int type)
{
	StorageType storageType = static_cast<StorageType>(type);

	auto storageIterator = m_sequencedStorageMap.find(storageType);

	if (storageIterator != m_sequencedStorageMap.end())
	{
		auto&[storageTypeFromIterator, collection] = *storageIterator;
		Q_UNUSED(storageTypeFromIterator);

		collection.pushBack(parsedPagePtr);

		emit parsedPageAdded(storage(storageType)->size() - 1, static_cast<int>(storageType));
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

		collection.clear();
	}

	SequencedStorage& sequencedStorage = m_sequencedStorageMap[StorageType::CrawledUrlStorageType];

	ASSERT(!sequencedStorage.containsPointersWithUseCountGreaterThanOne());

	sequencedStorage.clear();

	emit endClearData();
}

void SequencedDataCollection::initializeStorages()
{
	m_sequencedStorageMap = std::initializer_list<std::pair<const int, SequencedStorage>>
	{
		std::make_pair(StorageType::CrawledUrlStorageType, SequencedStorage()),
		std::make_pair(StorageType::ExternalUrlStorageType, SequencedStorage()),

		std::make_pair(StorageType::UpperCaseUrlStorageType, SequencedStorage()),
		std::make_pair(StorageType::NonAsciiCharacterUrlStorageType, SequencedStorage()),
		std::make_pair(StorageType::VeryLongUrlStorageType, SequencedStorage()),

		std::make_pair(StorageType::EmptyTitleUrlStorageType, SequencedStorage()),
		std::make_pair(StorageType::DuplicatedTitleUrlStorageType, SequencedStorage()),
		std::make_pair(StorageType::VeryLongTitleUrlStorageType, SequencedStorage()),
		std::make_pair(StorageType::VeryShortTitleUrlStorageType, SequencedStorage()),
		std::make_pair(StorageType::DuplicatedH1TitleUrlStorageType, SequencedStorage()),
		std::make_pair(StorageType::SeveralTitleUrlStorageType, SequencedStorage()),

		std::make_pair(StorageType::EmptyMetaDescriptionUrlStorageType, SequencedStorage()),
		std::make_pair(StorageType::DuplicatedMetaDescriptionUrlStorageType, SequencedStorage()),
		std::make_pair(StorageType::VeryLongMetaDescriptionUrlStorageType, SequencedStorage()),
		std::make_pair(StorageType::VeryShortMetaDescriptionUrlStorageType, SequencedStorage()),
		std::make_pair(StorageType::SeveralMetaDescriptionUrlStorageType, SequencedStorage()),

		std::make_pair(StorageType::EmptyMetaKeywordsUrlStorageType, SequencedStorage()),
		std::make_pair(StorageType::DuplicatedMetaKeywordsUrlStorageType, SequencedStorage()),
		std::make_pair(StorageType::SeveralMetaKeywordsUrlStorageType, SequencedStorage()),

		std::make_pair(StorageType::MissingH1UrlStorageType, SequencedStorage()),
		std::make_pair(StorageType::DuplicatedH1UrlStorageType, SequencedStorage()),
		std::make_pair(StorageType::VeryLongH1UrlStorageType, SequencedStorage()),
		std::make_pair(StorageType::SeveralH1UrlStorageType, SequencedStorage()),

		std::make_pair(StorageType::MissingH2UrlStorageType, SequencedStorage()),
		std::make_pair(StorageType::DuplicatedH2UrlStorageType, SequencedStorage()),
		std::make_pair(StorageType::VeryLongH2UrlStorageType, SequencedStorage()),
		std::make_pair(StorageType::SeveralH2UrlStorageType, SequencedStorage()),

		std::make_pair(StorageType::Over100kbImageStorageType, SequencedStorage()),
		std::make_pair(StorageType::MissingAltTextImageStorageType, SequencedStorage()),
		std::make_pair(StorageType::VeryLongAltTextImageStorageType, SequencedStorage()),
		std::make_pair(StorageType::Status404StorageType, SequencedStorage()),

		std::make_pair(StorageType::HtmlResourcesStorageType, SequencedStorage()),
		std::make_pair(StorageType::ImageResourcesStorageType, SequencedStorage()),
		std::make_pair(StorageType::JavaScriptResourcesStorageType, SequencedStorage()),
		std::make_pair(StorageType::StyleSheetResourcesStorageType, SequencedStorage()),
		std::make_pair(StorageType::FlashResourcesStorageType, SequencedStorage()),
		std::make_pair(StorageType::VideoResourcesStorageType, SequencedStorage()),
		std::make_pair(StorageType::OtherResourcesStorageType, SequencedStorage()),

		std::make_pair(StorageType::ExternalHtmlResourcesStorageType, SequencedStorage()),
		std::make_pair(StorageType::ExternalImageResourcesStorageType, SequencedStorage()),
		std::make_pair(StorageType::ExternalJavaScriptResourcesStorageType, SequencedStorage()),
		std::make_pair(StorageType::ExternalStyleSheetResourcesStorageType, SequencedStorage()),
		std::make_pair(StorageType::ExternalFlashResourcesStorageType, SequencedStorage()),
		std::make_pair(StorageType::ExternalVideoResourcesStorageType, SequencedStorage()),
		std::make_pair(StorageType::ExternalOtherResourcesStorageType, SequencedStorage())
	};
}

}