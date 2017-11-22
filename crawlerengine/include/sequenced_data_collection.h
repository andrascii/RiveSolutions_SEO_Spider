#pragma once

#include "parsed_page.h"
#include "parsed_page_hasher_proxy.h"
#include "parsed_page_comparator.h"
#include "unordered_data_collection.h"
#include "storage_type.h"
#include "isequenced_storage.h"

namespace CrawlerEngine
{

class SequencedDataCollection : public QObject
{
	Q_OBJECT

public:
	SequencedDataCollection();

	bool empty() const noexcept;

	const ISequencedStorage* storage(StorageType type) const noexcept;

	size_t crawledStorageSize() const;

signals:
	void parsedPageAdded(int row, int storageType);

	void parsedPageLinksToThisResourceChanged(LinksToThisResourceChanges changes);

	void beginClearData();

	void endClearData();

protected:
	virtual std::shared_ptr<ISequencedStorage> createSequencedStorage() const;

	void initializeStorages();

protected slots:
	void addParsedPage(ParsedPagePtr parsedPagePtr, int type);

	void onDataCleared();

private:
	ISequencedStorage* storage(StorageType type) noexcept;

private:
	std::unordered_map<int, std::shared_ptr<ISequencedStorage>> m_sequencedStorageMap;

	friend class UnorderedDataCollection;

	std::atomic<size_t> m_crawledStorageSize;
};

}