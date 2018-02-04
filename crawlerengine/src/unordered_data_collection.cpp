#include "unordered_data_collection.h"
#include "parsed_page_comparator.h"
#include "sequenced_data_collection.h"

namespace CrawlerEngine
{

UnorderedDataCollection::UnorderedDataCollection(QObject* parent)
	: QObject(parent)
{
	qRegisterMetaType<StorageType>("StorageType");

	initializeStorages();
}

bool UnorderedDataCollection::isParsedPageExists(const ParsedPagePtr& parsedPagePtr, StorageType type) const noexcept
{
	checkStorageType(type);

	const UnorderedStorageType& unorderedStorage = storage(type);
	return unorderedStorage.find(parsedPagePtr) != unorderedStorage.end();
}

void UnorderedDataCollection::replaceParsedPage(const ParsedPagePtr& oldPage, const ParsedPagePtr& newPage, StorageType type)
{
	checkStorageType(type);

	std::vector<bool>& pageStoragesFlags = newPage->storages;

	if (pageStoragesFlags.size() < static_cast<size_t>(StorageType::EndEnumStorageType))
	{
		pageStoragesFlags.resize(static_cast<size_t>(StorageType::EndEnumStorageType), false);
	}

	pageStoragesFlags[static_cast<size_t>(type)] = true;

	const auto removedItemsCount = storage(type).erase(oldPage);

	DEBUG_ASSERT(removedItemsCount == 1);

	storage(type).insert(newPage);

	DEBUG_ASSERT(isParsedPageExists(newPage, type));

	emit parsedPageReplaced(oldPage, newPage, type);
}

const ParsedPagePtr UnorderedDataCollection::parsedPage(const ParsedPagePtr& parsedPagePtr, StorageType type) const noexcept
{
	checkStorageType(type);

	const UnorderedStorageType& unorderedStorage = storage(type);
	auto iter = unorderedStorage.find(parsedPagePtr);

	return iter != unorderedStorage.end() ? *iter : ParsedPagePtr();
}

void UnorderedDataCollection::clearData()
{
	initializeStorages();

	emit dataCleared();
}

std::vector<ParsedPagePtr> UnorderedDataCollection::allParsedPages(StorageType type) const
{
	const UnorderedStorageType& container = storage(type);

	std::vector<ParsedPagePtr> result;
	result.reserve(container.size());

	for (auto it = container.begin(); it != container.end(); ++it)
	{
		result.push_back(*it);
	}

	return result;
}

void UnorderedDataCollection::addParsedPage(const ParsedPagePtr& parsedPagePtr, StorageType type)
{
	std::vector<bool>& pageStoragesFlags = parsedPagePtr->storages;

	if (pageStoragesFlags.size() < static_cast<size_t>(StorageType::EndEnumStorageType))
	{
		pageStoragesFlags.resize(static_cast<size_t>(StorageType::EndEnumStorageType), false);
	}

	pageStoragesFlags[static_cast<size_t>(type)] = true;
	storage(type).insert(parsedPagePtr);

	DEBUG_ASSERT(isParsedPageExists(parsedPagePtr, type));

	emit parsedPageAdded(parsedPagePtr, type);
}

void UnorderedDataCollection::addParsedPage(ParsedPagePtr parsedPagePtr, int type)
{
	const StorageType storage = static_cast<StorageType>(type);
	addParsedPage(parsedPagePtr, storage);
}

ParsedPagePtr UnorderedDataCollection::removeParsedPage(const ParsedPagePtr& parsedPagePtr, StorageType type) noexcept
{
	checkStorageType(type);

	UnorderedStorageType& unorderedStorage = storage(type);

	auto iter = unorderedStorage.find(parsedPagePtr);

	if (iter != unorderedStorage.end())
	{
		ParsedPagePtr result = std::move(*iter);
		result->storages[static_cast<size_t>(type)] = false;

		unorderedStorage.erase(iter);

		return result;
	}

	return ParsedPagePtr();
}

UnorderedDataCollection::UnorderedStorageType& UnorderedDataCollection::storage(StorageType type) noexcept
{
	const UnorderedStorageType& storage = static_cast<const UnorderedDataCollection* const>(this)->storage(type);

	return const_cast<UnorderedStorageType&>(storage);
}

const UnorderedDataCollection::UnorderedStorageType& UnorderedDataCollection::storage(StorageType type) const noexcept
{
	ASSERT(m_unorderedStorageMap.find(type) != m_unorderedStorageMap.end());

	return m_unorderedStorageMap.find(type)->second;
}

void UnorderedDataCollection::checkStorageType(StorageType type) const noexcept
{
	ASSERT(type > StorageType::BeginEnumStorageType && type < StorageType::EndEnumStorageType);
}

void UnorderedDataCollection::initializeStorages()
{
	m_unorderedStorageMap = std::initializer_list<std::pair<const StorageType, UnorderedStorageType>>
	{
		std::make_pair(StorageType::CrawledUrlStorageType,
			UnorderedStorageType(0, ParsedPageHasherProxy(new ParsedPageHasherUrl), 
				ParsedPageComparatorProxy(new ParsedPageUrlComparator))),

		// Link Problems Storages

		std::make_pair(StorageType::ExternalUrlStorageType,
			UnorderedStorageType(0, ParsedPageHasherProxy(new ParsedPageHasherUrl), 
				ParsedPageComparatorProxy(new ParsedPageUrlComparator))),

		std::make_pair(StorageType::UpperCaseUrlStorageType,
			UnorderedStorageType(0, ParsedPageHasherProxy(new ParsedPageHasherUrl), 
				ParsedPageComparatorProxy(new ParsedPageUrlComparator))),

		std::make_pair(StorageType::NonAsciiCharacterUrlStorageType, 
			UnorderedStorageType(0, ParsedPageHasherProxy(new ParsedPageHasherUrl), 
				ParsedPageComparatorProxy(new ParsedPageUrlComparator))),

		std::make_pair(StorageType::TooLongUrlStorageType,
			UnorderedStorageType(0, ParsedPageHasherProxy(new ParsedPageHasherUrl), 
				ParsedPageComparatorProxy(new ParsedPageUrlComparator))),

		std::make_pair(StorageType::BrokenLinks,
			UnorderedStorageType(0, ParsedPageHasherProxy(new ParsedPageHasherUrl),
				ParsedPageComparatorProxy(new ParsedPageUrlComparator))),

		std::make_pair(StorageType::WwwRedirectionsUrlStorageType,
			UnorderedStorageType(0, ParsedPageHasherProxy(new ParsedPageHasherUrl),
				ParsedPageComparatorProxy(new ParsedPageUrlComparator))),

		std::make_pair(StorageType::Status4xxStorageType,
			UnorderedStorageType(0, ParsedPageHasherProxy(new ParsedPageHasherUrl),
				ParsedPageComparatorProxy(new ParsedPageUrlComparator))),

		std::make_pair(StorageType::Status5xxStorageType,
			UnorderedStorageType(0, ParsedPageHasherProxy(new ParsedPageHasherUrl),
				ParsedPageComparatorProxy(new ParsedPageUrlComparator))),

		std::make_pair(StorageType::Status302StorageType,
			UnorderedStorageType(0, ParsedPageHasherProxy(new ParsedPageHasherUrl),
				ParsedPageComparatorProxy(new ParsedPageUrlComparator))),

		std::make_pair(StorageType::Status301StorageType,
			UnorderedStorageType(0, ParsedPageHasherProxy(new ParsedPageHasherUrl),
				ParsedPageComparatorProxy(new ParsedPageUrlComparator))),

		std::make_pair(StorageType::CanonicalUrlResourcesStorageType,
			UnorderedStorageType(0, ParsedPageHasherProxy(new ParsedPageHasherCanonicalLinkElement),
				ParsedPageComparatorProxy(new ParsedPageCanonicalLinkElementComparator))),

		std::make_pair(StorageType::UniqueCanonicalUrlResourcesStorageType,
			UnorderedStorageType(0, ParsedPageHasherProxy(new ParsedPageHasherCanonicalLinkElement),
				ParsedPageComparatorProxy(new ParsedPageCanonicalLinkElementComparator))),

		std::make_pair(StorageType::ExternalDoFollowUrlResourcesStorageType,
			UnorderedStorageType(0, ParsedPageHasherProxy(new ParsedPageHasherUrl),
				ParsedPageComparatorProxy(new ParsedPageUrlComparator))),

		// Title Storages

		std::make_pair(StorageType::AllTitlesUrlStorageType,
			UnorderedStorageType(0, ParsedPageHasherProxy(new ParsedPageHasherTitle), 
				ParsedPageComparatorProxy(new ParsedPageTitleComparator))),

		std::make_pair(StorageType::EmptyTitleUrlStorageType,
			UnorderedStorageType(0, ParsedPageHasherProxy(new ParsedPageHasherTitle), 
				ParsedPageComparatorProxy(new ParsedPageTitleComparator))),

		std::make_pair(StorageType::DuplicatedTitleUrlStorageType,
			UnorderedStorageType(0, ParsedPageHasherProxy(new ParsedPageHasherTitle),
				ParsedPageComparatorProxy(new ParsedPageTitleComparator))),

		std::make_pair(StorageType::TooLongTitleUrlStorageType,
			UnorderedStorageType(0, ParsedPageHasherProxy(new ParsedPageHasherTitle), 
				ParsedPageComparatorProxy(new ParsedPageTitleComparator))),

		std::make_pair(StorageType::TooShortTitleUrlStorageType,
			UnorderedStorageType(0, ParsedPageHasherProxy(new ParsedPageHasherTitle), 
				ParsedPageComparatorProxy(new ParsedPageTitleComparator))),

		std::make_pair(StorageType::DuplicatedH1TitleUrlStorageType,
			UnorderedStorageType(0, ParsedPageHasherProxy(new ParsedPageHasherTitle),
				ParsedPageComparatorProxy(new ParsedPageTitleComparator))),

		std::make_pair(StorageType::SeveralTitleUrlStorageType,
			UnorderedStorageType(0, ParsedPageHasherProxy(new ParsedPageHasherTitle), 
				ParsedPageComparatorProxy(new ParsedPageTitleComparator))),

		// Meta Description Storages

		std::make_pair(StorageType::AllMetaDescriptionsUrlStorageType,
			UnorderedStorageType(0, ParsedPageHasherProxy(new ParsedPageHasherMetaDescription), 
				ParsedPageComparatorProxy(new ParsedPageMetaDescriptionComparator))),

		std::make_pair(StorageType::EmptyMetaDescriptionUrlStorageType,
			UnorderedStorageType(0, ParsedPageHasherProxy(new ParsedPageHasherMetaDescription), 
				ParsedPageComparatorProxy(new ParsedPageMetaDescriptionComparator))),

		std::make_pair(StorageType::DuplicatedMetaDescriptionUrlStorageType,
			UnorderedStorageType(0, ParsedPageHasherProxy(new ParsedPageHasherMetaDescription),
				ParsedPageComparatorProxy(new ParsedPageMetaDescriptionComparator))),

		std::make_pair(StorageType::TooLongMetaDescriptionUrlStorageType,
			UnorderedStorageType(0, ParsedPageHasherProxy(new ParsedPageHasherMetaDescription), 
				ParsedPageComparatorProxy(new ParsedPageMetaDescriptionComparator))),

		std::make_pair(StorageType::TooShortMetaDescriptionUrlStorageType,
			UnorderedStorageType(0, ParsedPageHasherProxy(new ParsedPageHasherMetaDescription), 
				ParsedPageComparatorProxy(new ParsedPageMetaDescriptionComparator))),

		std::make_pair(StorageType::SeveralMetaDescriptionUrlStorageType,
			UnorderedStorageType(0, ParsedPageHasherProxy(new ParsedPageHasherMetaDescription), 
				ParsedPageComparatorProxy(new ParsedPageMetaDescriptionComparator))),

		// Meta Keywords Problems Storages

		std::make_pair(StorageType::AllMetaKeywordsUrlStorageType,
			UnorderedStorageType(0, ParsedPageHasherProxy(new ParsedPageHasherMetaKeywords), 
				ParsedPageComparatorProxy(new ParsedPageMetaKeywordsComparator))),

		std::make_pair(StorageType::EmptyMetaKeywordsUrlStorageType,
			UnorderedStorageType(0, ParsedPageHasherProxy(new ParsedPageHasherMetaKeywords), 
				ParsedPageComparatorProxy(new ParsedPageMetaKeywordsComparator))),

		std::make_pair(StorageType::DuplicatedMetaKeywordsUrlStorageType,
			UnorderedStorageType(0, ParsedPageHasherProxy(new ParsedPageHasherMetaKeywords),
				ParsedPageComparatorProxy(new ParsedPageMetaKeywordsComparator))),

		std::make_pair(StorageType::SeveralMetaKeywordsUrlStorageType,
			UnorderedStorageType(0, ParsedPageHasherProxy(new ParsedPageHasherMetaKeywords), 
				ParsedPageComparatorProxy(new ParsedPageMetaKeywordsComparator))),

		// H1 Problems Storages

		std::make_pair(StorageType::AllH1UrlStorageType,
			UnorderedStorageType(0, ParsedPageHasherProxy(new ParsedPageHasherFirstH1), 
				ParsedPageComparatorProxy(new ParsedPageFirstH1Comparator))),

		std::make_pair(StorageType::MissingH1UrlStorageType,
			UnorderedStorageType(0, ParsedPageHasherProxy(new ParsedPageHasherFirstH1), 
				ParsedPageComparatorProxy(new ParsedPageFirstH1Comparator))),

		std::make_pair(StorageType::DuplicatedH1UrlStorageType,
			UnorderedStorageType(0, ParsedPageHasherProxy(new ParsedPageHasherFirstH1), 
				ParsedPageComparatorProxy(new ParsedPageFirstH1Comparator))),

		std::make_pair(StorageType::TooLongH1UrlStorageType,
			UnorderedStorageType(0, ParsedPageHasherProxy(new ParsedPageHasherFirstH1), 
				ParsedPageComparatorProxy(new ParsedPageFirstH1Comparator))),

		std::make_pair(StorageType::SeveralH1UrlStorageType,
			UnorderedStorageType(0, ParsedPageHasherProxy(new ParsedPageHasherFirstH1), 
				ParsedPageComparatorProxy(new ParsedPageFirstH1Comparator))),

		// H2 Problems Storages

		std::make_pair(StorageType::AllH2UrlStorageType,
			UnorderedStorageType(0, ParsedPageHasherProxy(new ParsedPageHasherFirstH2), 
				ParsedPageComparatorProxy(new ParsedPageFirstH2Comparator))),

		std::make_pair(StorageType::MissingH2UrlStorageType,
			UnorderedStorageType(0, ParsedPageHasherProxy(new ParsedPageHasherFirstH2), 
				ParsedPageComparatorProxy(new ParsedPageFirstH2Comparator))),

		std::make_pair(StorageType::DuplicatedH2UrlStorageType,
			UnorderedStorageType(0, ParsedPageHasherProxy(new ParsedPageHasherFirstH2),
				ParsedPageComparatorProxy(new ParsedPageFirstH2Comparator))),

		std::make_pair(StorageType::TooLongH2UrlStorageType,
			UnorderedStorageType(0, ParsedPageHasherProxy(new ParsedPageHasherFirstH2), 
				ParsedPageComparatorProxy(new ParsedPageFirstH2Comparator))),

		std::make_pair(StorageType::SeveralH2UrlStorageType,
			UnorderedStorageType(0, ParsedPageHasherProxy(new ParsedPageHasherFirstH2), 
				ParsedPageComparatorProxy(new ParsedPageFirstH2Comparator))),

		// Images Problems Storages

		std::make_pair(StorageType::Over100kbImageStorageType,
			UnorderedStorageType(0, ParsedPageHasherProxy(new ParsedPageHasherUrl), 
				ParsedPageComparatorProxy(new ParsedPageUrlComparator))),

		std::make_pair(StorageType::MissingAltTextImageStorageType,
			UnorderedStorageType(0, ParsedPageHasherProxy(new ParsedPageHasherUrl), 
				ParsedPageComparatorProxy(new ParsedPageUrlComparator))),

		std::make_pair(StorageType::TooLongAltTextImageStorageType,
			UnorderedStorageType(0, ParsedPageHasherProxy(new ParsedPageHasherUrl), 
				ParsedPageComparatorProxy(new ParsedPageUrlComparator))),

		// Page problems

		std::make_pair(StorageType::TooManyLinksOnPageStorageType,
			UnorderedStorageType(0, ParsedPageHasherProxy(new ParsedPageHasherUrl),
				ParsedPageComparatorProxy(new ParsedPageUrlComparator))),

		std::make_pair(StorageType::ContainsMetaRefreshTagStorageType,
			UnorderedStorageType(0, ParsedPageHasherProxy(new ParsedPageHasherUrl),
				ParsedPageComparatorProxy(new ParsedPageUrlComparator))),

		std::make_pair(StorageType::ContainsFramesStorageType,
			UnorderedStorageType(0, ParsedPageHasherProxy(new ParsedPageHasherUrl),
				ParsedPageComparatorProxy(new ParsedPageUrlComparator))),

		// Resources Types Storages

		std::make_pair(StorageType::PendingResourcesStorageType,
			UnorderedStorageType(0, ParsedPageHasherProxy(new ParsedPageHasherUrl), 
				ParsedPageComparatorProxy(new ParsedPageUrlComparator))),

		std::make_pair(StorageType::HtmlResourcesStorageType,
			UnorderedStorageType(0, ParsedPageHasherProxy(new ParsedPageHasherUrl), 
				ParsedPageComparatorProxy(new ParsedPageUrlComparator))),

		std::make_pair(StorageType::ImageResourcesStorageType,
			UnorderedStorageType(0, ParsedPageHasherProxy(new ParsedPageHasherUrl), 
				ParsedPageComparatorProxy(new ParsedPageUrlComparator))),

		std::make_pair(StorageType::JavaScriptResourcesStorageType,
			UnorderedStorageType(0, ParsedPageHasherProxy(new ParsedPageHasherUrl), 
				ParsedPageComparatorProxy(new ParsedPageUrlComparator))),

		std::make_pair(StorageType::StyleSheetResourcesStorageType,
			UnorderedStorageType(0, ParsedPageHasherProxy(new ParsedPageHasherUrl), 
				ParsedPageComparatorProxy(new ParsedPageUrlComparator))),

		std::make_pair(StorageType::FlashResourcesStorageType,
			UnorderedStorageType(0, ParsedPageHasherProxy(new ParsedPageHasherUrl), 
				ParsedPageComparatorProxy(new ParsedPageUrlComparator))),

		std::make_pair(StorageType::VideoResourcesStorageType,
			UnorderedStorageType(0, ParsedPageHasherProxy(new ParsedPageHasherUrl), 
				ParsedPageComparatorProxy(new ParsedPageUrlComparator))),

		std::make_pair(StorageType::OtherResourcesStorageType,
			UnorderedStorageType(0, ParsedPageHasherProxy(new ParsedPageHasherUrl), 
				ParsedPageComparatorProxy(new ParsedPageUrlComparator))),

		std::make_pair(StorageType::ExternalHtmlResourcesStorageType,
			UnorderedStorageType(0, ParsedPageHasherProxy(new ParsedPageHasherUrl),
				ParsedPageComparatorProxy(new ParsedPageUrlComparator))),

		std::make_pair(StorageType::ExternalImageResourcesStorageType,
			UnorderedStorageType(0, ParsedPageHasherProxy(new ParsedPageHasherUrl),
				ParsedPageComparatorProxy(new ParsedPageUrlComparator))),

		std::make_pair(StorageType::ExternalJavaScriptResourcesStorageType,
			UnorderedStorageType(0, ParsedPageHasherProxy(new ParsedPageHasherUrl),
				ParsedPageComparatorProxy(new ParsedPageUrlComparator))),

		std::make_pair(StorageType::ExternalStyleSheetResourcesStorageType,
			UnorderedStorageType(0, ParsedPageHasherProxy(new ParsedPageHasherUrl),
				ParsedPageComparatorProxy(new ParsedPageUrlComparator))),

		std::make_pair(StorageType::ExternalFlashResourcesStorageType,
			UnorderedStorageType(0, ParsedPageHasherProxy(new ParsedPageHasherUrl),
				ParsedPageComparatorProxy(new ParsedPageUrlComparator))),

		std::make_pair(StorageType::ExternalVideoResourcesStorageType,
			UnorderedStorageType(0, ParsedPageHasherProxy(new ParsedPageHasherUrl),
				ParsedPageComparatorProxy(new ParsedPageUrlComparator))),

		std::make_pair(StorageType::ExternalOtherResourcesStorageType,
			UnorderedStorageType(0, ParsedPageHasherProxy(new ParsedPageHasherUrl),
				ParsedPageComparatorProxy(new ParsedPageUrlComparator))),
	};
}

}