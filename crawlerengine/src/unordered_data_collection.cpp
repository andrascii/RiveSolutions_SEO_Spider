#include "unordered_data_collection.h"
#include "parsed_page_comparator.h"
#include "sequenced_data_collection.h"

namespace CrawlerEngine
{

UnorderedDataCollection::UnorderedDataCollection(QObject* parent)
	: QObject(parent)
	, m_emitAbility(true)
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

size_t UnorderedDataCollection::size(StorageType type) const noexcept
{
	checkStorageType(type);

	const UnorderedStorageType& unorderedStorage = storage(type);
	return unorderedStorage.size();
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

void UnorderedDataCollection::setPageAddingEmitAbility(bool value)
{
	ASSERT(thread() == QThread::currentThread());

	m_emitAbility = value;

	if (!m_emitAbility)
	{
		return;
	}

	for (const auto& emitData : m_emitWorkerResultData)
	{
		emit parsedPageAdded(emitData.object, emitData.type);
	}

	for (const auto& emitData : m_emitParsedPagePtrData)
	{
		emit parsedPageAdded(emitData.object, emitData.type);
	}

	m_emitWorkerResultData.clear();
	m_emitParsedPagePtrData.clear();
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

void UnorderedDataCollection::prepareCollectionForRefreshPage(const ParsedPagePtr& pageForRefresh)
{
	using ItemIterator = UnorderedStorageType::iterator;

	const auto itemDeleter = [this](UnorderedStorageType& storage, StorageType type, ItemIterator iter)
	{
		emit parsedPageRemoved(*iter, type);

		return storage.erase(iter);
	};

	const auto itemsDeleter = [&itemDeleter](UnorderedStorageType& storage, StorageType type, ItemIterator first, ItemIterator last)
	{
		while(first != last)
		{
			first = itemDeleter(storage, type, first);
		}
	};

	const auto binaryRemove = [&](UnorderedStorageType& storage, StorageType type, std::pair<ItemIterator, ItemIterator>& range)
	{
		for (auto it = range.first; it != range.second; ++it)
		{
			if (pageForRefresh != *it)
			{
				continue;
			}

			DEBUG_ASSERT((*it)->storages[static_cast<size_t>(type)]);
			(*it)->storages[static_cast<size_t>(type)] = false;

			if (it == range.first)
			{
				range.first = itemDeleter(storage, type, it);
			}
			else
			{
				itemDeleter(storage, type, it);
			}

			break;
		}
	};

	const auto removeItemsIfAll = [&](UnorderedStorageType& storage, StorageType type, const std::pair<ItemIterator, ItemIterator>& range, auto&& predicate)
	{
		if (!std::distance(range.first, range.second))
		{
			return;
		}

		bool needToRemove = true;

		auto last = range.first;
		++last;

		for (auto first = range.first; last != range.second; ++first, ++last)
		{
			if (!predicate(*first, *last))
			{
				needToRemove = false;
				break;
			}
		}

		if (!needToRemove)
		{
			return;
		}

		for (auto it = range.first; it != range.second; ++it)
		{
			DEBUG_ASSERT((*it)->storages[static_cast<size_t>(type)]);
			(*it)->storages[static_cast<size_t>(type)] = false;
		}

		itemsDeleter(storage, type, range.first, range.second);
	};

	for (auto&[type, storage] : m_unorderedStorageMap)
	{
		if (type == StorageType::CrawledUrlStorageType ||
			type == StorageType::PendingResourcesStorageType)
		{
			continue;
		}

		std::pair<ItemIterator, ItemIterator> range = storage.equal_range(pageForRefresh);

		binaryRemove(storage, type, range);

		if (type == StorageType::DuplicatedTitleUrlStorageType ||
			type == StorageType::DuplicatedMetaDescriptionUrlStorageType ||
			type == StorageType::DuplicatedMetaKeywordsUrlStorageType ||
			type == StorageType::DuplicatedH1UrlStorageType ||
			type == StorageType::DuplicatedH2UrlStorageType)
		{
			const auto canonicalUrlComparator = [](const ParsedPagePtr& first, const ParsedPagePtr& second)
			{
				return first->canonicalUrl.canonizedUrlStr() == second->canonicalUrl.canonizedUrlStr();
			};

			removeItemsIfAll(storage, type, range, canonicalUrlComparator);
		}
	}
}

void UnorderedDataCollection::addParsedPage(WorkerResult& workerResult, StorageType type)
{
	addParsedPageInternal(workerResult.incomingPage(), type);

	emit parsedPageAdded(workerResult, type);
}

void UnorderedDataCollection::addParsedPage(WorkerResult workerResult, int type)
{
	const StorageType storage = static_cast<StorageType>(type);
	addParsedPage(workerResult, storage);
}

void UnorderedDataCollection::addParsedPage(ParsedPagePtr& parsedPagePointer, StorageType type)
{
	addParsedPageInternal(parsedPagePointer, type);

	emit parsedPageAdded(parsedPagePointer, type);
}

void UnorderedDataCollection::addParsedPage(ParsedPagePtr parsedPagePointer, int type)
{
	const StorageType storage = static_cast<StorageType>(type);
	addParsedPage(parsedPagePointer, storage);
}

ParsedPagePtr UnorderedDataCollection::removeParsedPage(const ParsedPagePtr& parsedPagePtr, StorageType type) noexcept
{
	return removeParsedPageInternal(parsedPagePtr, type).first;
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

		std::make_pair(StorageType::ExternalUrlStorageType,
			UnorderedStorageType(0, ParsedPageHasherProxy(new ParsedPageHasherUrl),
				ParsedPageComparatorProxy(new ParsedPageUrlComparator))),

		std::make_pair(StorageType::DofollowUrlStorageType,
			UnorderedStorageType(0, ParsedPageHasherProxy(new ParsedPageHasherUrl),
				ParsedPageComparatorProxy(new ParsedPageUrlComparator))),

		// Link Problems Storages

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

		std::make_pair(StorageType::TooManyRedirectsStorageType,
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

		std::make_pair(StorageType::AllCanonicalUrlResourcesStorageType,
			UnorderedStorageType(0, ParsedPageHasherProxy(new ParsedPageHasherCanonicalLinkElement),
				ParsedPageComparatorProxy(new ParsedPageCanonicalLinkElementComparator))),
		
		std::make_pair(StorageType::DuplicatedCanonicalUrlResourcesStorageType,
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

		std::make_pair(StorageType::TooBigImageStorageType,
			UnorderedStorageType(0, ParsedPageHasherProxy(new ParsedPageHasherUrl), 
				ParsedPageComparatorProxy(new ParsedPageUrlComparator))),

		std::make_pair(StorageType::MissingAltTextImageStorageType,
			UnorderedStorageType(0, ParsedPageHasherProxy(new ParsedPageHasherUrl), 
				ParsedPageComparatorProxy(new ParsedPageUrlComparator))),

		std::make_pair(StorageType::TooLongAltTextImageStorageType,
			UnorderedStorageType(0, ParsedPageHasherProxy(new ParsedPageHasherUrl), 
				ParsedPageComparatorProxy(new ParsedPageUrlComparator))),

		std::make_pair(StorageType::BrokenImagesStorageType,
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

		std::make_pair(StorageType::TooBigHtmlResourcesStorageType,
			UnorderedStorageType(0, ParsedPageHasherProxy(new ParsedPageHasherUrl),
				ParsedPageComparatorProxy(new ParsedPageUrlComparator))),

		// not indexed pages

		std::make_pair(StorageType::BlockedForSEIndexingStorageType,
			UnorderedStorageType(0, ParsedPageHasherProxy(new ParsedPageHasherUrl),
				ParsedPageComparatorProxy(new ParsedPageUrlComparator))),

		std::make_pair(StorageType::NofollowLinksStorageType,
			UnorderedStorageType(0, ParsedPageHasherProxy(new ParsedPageHasherUrl),
				ParsedPageComparatorProxy(new ParsedPageUrlComparator))),

		std::make_pair(StorageType::BlockedByRobotsTxtStorageType,
			UnorderedStorageType(0, ParsedPageHasherProxy(new ParsedPageHasherUrl),
				ParsedPageComparatorProxy(new ParsedPageUrlComparator))),

		std::make_pair(StorageType::BlockedByXRobotsTagStorageType,
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


void UnorderedDataCollection::addParsedPageInternal(ParsedPagePtr& parsedPagePointer, StorageType type)
{
	std::vector<bool>& pageStoragesFlags = parsedPagePointer->storages;

	if (pageStoragesFlags.size() < static_cast<size_t>(StorageType::EndEnumStorageType))
	{
		pageStoragesFlags.resize(static_cast<size_t>(StorageType::EndEnumStorageType), false);
	}

	pageStoragesFlags[static_cast<size_t>(type)] = true;
	storage(type).insert(parsedPagePointer);

	DEBUG_ASSERT(isParsedPageExists(parsedPagePointer, type));
}


std::pair<ParsedPagePtr, UnorderedDataCollection::UnorderedStorageType::iterator> 
UnorderedDataCollection::removeParsedPageInternal(const ParsedPagePtr& parsedPagePtr, StorageType type) noexcept
{
	checkStorageType(type);

	UnorderedStorageType& unorderedStorage = storage(type);

	auto iter = unorderedStorage.find(parsedPagePtr);

	if (iter != unorderedStorage.end())
	{
		ParsedPagePtr result = std::move(*iter);
		result->storages[static_cast<size_t>(type)] = false;

		const auto nextItem = unorderedStorage.erase(iter);

		emit parsedPageRemoved(result, type);

		return std::make_pair(result, nextItem);
	}

	return std::make_pair(ParsedPagePtr(), unorderedStorage.end());
}


void UnorderedDataCollection::emitParsedPageAdded(WorkerResult workerResult, StorageType type)
{
	if (!m_emitAbility)
	{
		return;
	}

	m_emitWorkerResultData.push_back(EmitData<WorkerResult>{ workerResult, type });
}

void UnorderedDataCollection::emitParsedPageAdded(ParsedPagePtr parsedPagePointer, StorageType type)
{
	if (!m_emitAbility)
	{
		return;
	}

	m_emitParsedPagePtrData.push_back(EmitData<ParsedPagePtr>{ parsedPagePointer, type });
}

}