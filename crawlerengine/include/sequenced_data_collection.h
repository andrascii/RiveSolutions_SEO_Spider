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
	SequencedDataCollection(const UnorderedDataCollection* collection);

	void initialize();
	bool empty() const noexcept;
	
	const ISequencedStorage* storage(StorageType type) const noexcept;
	ISequencedStorage* storage(StorageType type) noexcept;
	bool removePage(ParsedPage* parsedPage, StorageType type);
	void prepareCollectionForRefreshPage(ParsedPage* pageForRefresh);

signals:
	void parsedPageAdded(int row, StorageType type);
	void parsedPageReplaced(int row, StorageType type);
	void parsedPageRemoved(int row, StorageType type);
	void parsedPagesRemoved(int count, StorageType type);
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