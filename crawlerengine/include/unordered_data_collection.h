#pragma once

#include "parsed_page.h"
#include "parsed_page_hasher_proxy.h"
#include "parsed_page_comparator.h"
#include "storage_type.h"
#include "sequenced_data_collection_types.h"

namespace CrawlerEngine
{

struct LinksToThisResourceChanges;
class SequencedDataCollection;

class UnorderedDataCollection : public QObject
{
	Q_OBJECT

protected:
	using UnorderedStorageType = std::unordered_multiset<ParsedPagePtr, ParsedPageHasherProxy, ParsedPageComparatorProxy>;

public:
	UnorderedDataCollection(QObject* parent);

	bool isParsedPageExists(const ParsedPagePtr& parsedPagePtr, StorageType type) const noexcept;

	void addParsedPage(const ParsedPagePtr& parsedPagePtr, StorageType type) noexcept;
	Q_SLOT void addParsedPage(ParsedPagePtr parsedPagePtr, int type) noexcept;

	ParsedPagePtr removeParsedPage(const ParsedPagePtr& parsedPagePtr, StorageType type) noexcept;
	const ParsedPagePtr parsedPage(const ParsedPagePtr& parsedPagePtr, StorageType type) const noexcept;

	void clearData();

	std::vector<ParsedPagePtr> allParsedPages(StorageType type) const;

	template <class TPredicate>
	std::vector<ParsedPagePtr> allParsedPages(const ParsedPagePtr& parsedPagePtr, StorageType type, const TPredicate& unaryPredicate) const
	{
		std::vector<ParsedPagePtr> result;
		checkStorageType(type);

		const UnorderedStorageType& unorderedStorage = storage(type);
		const auto rangeIterators = unorderedStorage.equal_range(parsedPagePtr);

		for (auto it = rangeIterators.first; it != rangeIterators.second; ++it)
		{
			if (unaryPredicate(*it))
			{
				result.push_back(*it);
			}
		}

		return result;
	}

signals:
	void parsedPageAdded(ParsedPagePtr parsedPagePtr, StorageType type);
	void parsedPageLinksToThisResourceChanged(LinksToThisResourceChanges changes);
	void dataCleared();

protected:
	UnorderedStorageType& storage(StorageType type) noexcept;
	const UnorderedStorageType& storage(StorageType type) const noexcept;

private:
	void checkStorageType(StorageType type) const noexcept;
	void initializeStorages();

private:
	std::unordered_map<StorageType, UnorderedStorageType> m_unorderedStorageMap;
};

}
