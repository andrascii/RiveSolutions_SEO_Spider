#pragma once

#include "parsed_page.h"
#include "parsed_page_hasher_proxy.h"
#include "parsed_page_comparator.h"
#include "storage_type.h"
#include "sequenced_data_collection_types.h"
#include "worker_result.h"

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
	size_t size(StorageType type) const noexcept;

	void replaceParsedPage(const ParsedPagePtr& oldPage, const ParsedPagePtr& newPage, StorageType type, int turnaround);

	void addParsedPage(WorkerResult& workerResult, StorageType type);
	void addParsedPage(ParsedPagePtr& parsedPagePointer, StorageType type, int turnaround);

	Q_SLOT void addParsedPage(WorkerResult workerResult, int type);
	Q_SLOT void addParsedPage(ParsedPagePtr parsedPagePointer, int type, int turnaround);

	ParsedPagePtr removeParsedPage(const ParsedPagePtr& parsedPagePtr, StorageType type, int turnaround) noexcept;
	const ParsedPagePtr parsedPage(const ParsedPagePtr& parsedPagePtr, StorageType type) const noexcept;

	void setPageAddingEmitAbility(bool value, int turnaround);

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

	void prepareCollectionForRefreshPage(const ParsedPagePtr& pageForRefresh, int turnaround);

signals:
    void parsedPageAdded(CrawlerEngine::WorkerResult workerResult, CrawlerEngine::StorageType type);
	void parsedPageAdded(CrawlerEngine::ParsedPagePtr parsedPagePointer, CrawlerEngine::StorageType type, int turnaround);
	void parsedPageRemoved(CrawlerEngine::ParsedPagePtr parsedPagePointer, CrawlerEngine::StorageType type, int turnaround);
	void parsedPageReplaced(CrawlerEngine::ParsedPagePtr oldParsedPagePtr, CrawlerEngine::ParsedPagePtr newParsedPagePtr, CrawlerEngine::StorageType type, int turnaround);
	void parsedPageLinksToThisResourceChanged(CrawlerEngine::LinksToThisResourceChanges changes, int turnaround);
	void dataCleared();

protected:
	UnorderedStorageType& storage(StorageType type) noexcept;
	const UnorderedStorageType& storage(StorageType type) const noexcept;

private:
	void checkStorageType(StorageType type) const noexcept;
	void initializeStorages();
	void addParsedPageInternal(ParsedPagePtr& parsedPagePointer, StorageType type);
	std::pair<ParsedPagePtr, UnorderedStorageType::iterator> removeParsedPageInternal(const ParsedPagePtr& parsedPagePtr, StorageType type, int turnaround) noexcept;

	void emitParsedPageAdded(WorkerResult workerResult, StorageType type);
	void emitParsedPageAdded(ParsedPagePtr parsedPagePointer, StorageType type);

private:
	template <typename T>
	struct EmitData
	{
		T object;
		StorageType type;
	};

	std::unordered_map<StorageType, UnorderedStorageType> m_unorderedStorageMap;

	bool m_emitAbility;

	std::vector<EmitData<WorkerResult>> m_emitWorkerResultData;
	std::vector<EmitData<ParsedPagePtr>> m_emitParsedPagePtrData;
};

}
