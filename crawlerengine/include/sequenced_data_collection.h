#pragma once

#include "parsed_page.h"
#include "parsed_page_hasher_proxy.h"
#include "parsed_page_comparator.h"
#include "storage_type.h"
#include "sequenced_data_collection_types.h"
#include "worker_result.h"

namespace CrawlerEngine
{

class ISequencedStorage;
class UnorderedDataCollection;

class SequencedDataCollection : public QObject
{
	Q_OBJECT

public:
	friend class UnorderedDataCollection;

	SequencedDataCollection(const UnorderedDataCollection* collection);

	bool empty() const noexcept;
	
	const ISequencedStorage* storage(StorageType type) const noexcept;
	ISequencedStorage* storage(StorageType type) noexcept;

	bool removePage(ParsedPage* parsedPage, StorageType type);

	template <typename F>
	void removePageIf(StorageType type, F predicate)
	{
		ISequencedStorage* sequencedStorage = storage(type);

		for (int i = 0; i < sequencedStorage->size();)
		{
			if (!predicate(sequencedStorage->get(i)))
			{
				++i;
				continue;
			}

			if (!removePage(sequencedStorage->get(i), type))
			{
				++i;
			}
		}
	}

	void initialize();

signals:
	void parsedPageAdded(int row, StorageType type);
	void parsedPageReplaced(int row, StorageType type);
	void parsedPageRemoved(int row, StorageType type);
	void indicesRangeInvalidated(std::pair<int, int> indicesRange, StorageType type);
	void parsedPageLinksToThisResourceChanged(LinksToThisResourceChanges changes);
	void beginClearData();
	void endClearData();
	void refreshPageDone();

protected:
	virtual std::shared_ptr<ISequencedStorage> createSequencedStorage() const;

protected slots:
	void addParsedPage(ParsedPagePtr parsedPagePtr, StorageType type);
	void addParsedPage(WorkerResult workerResult, StorageType type);
	void replaceParsedPage(ParsedPagePtr oldParsedPagePtr, ParsedPagePtr newParsedPagePtr, StorageType type);
	void onDataCleared();

private:
	std::unordered_map<StorageType, std::shared_ptr<ISequencedStorage>> m_sequencedStorageMap;
};

}