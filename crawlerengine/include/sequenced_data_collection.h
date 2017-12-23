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
	bool empty() const noexcept;

	const ISequencedStorage* storage(StorageType type) const noexcept;

signals:
	void parsedPageAdded(int row, StorageType storageType);
	void parsedPageLinksToThisResourceChanged(LinksToThisResourceChanges changes);
	void beginClearData();
	void endClearData();

protected:
	virtual std::shared_ptr<ISequencedStorage> createSequencedStorage() const;

	void initializeStorages();

protected slots:
	void addParsedPage(ParsedPagePtr parsedPagePtr, StorageType type);
	void onDataCleared();

private:
	ISequencedStorage* storage(StorageType type) noexcept;

private:
	std::unordered_map<StorageType, std::shared_ptr<ISequencedStorage>> m_sequencedStorageMap;

	friend class UnorderedDataCollection;
};

}