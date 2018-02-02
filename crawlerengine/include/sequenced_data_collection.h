#pragma once

#include "parsed_page.h"
#include "parsed_page_hasher_proxy.h"
#include "parsed_page_comparator.h"
#include "storage_type.h"
#include "sequenced_data_collection_types.h"

namespace CrawlerEngine
{

class ISequencedStorage;
class UnorderedDataCollection;

class SequencedDataCollection : public QObject
{
	Q_OBJECT

public:
	SequencedDataCollection(const UnorderedDataCollection* collection);

	bool empty() const noexcept;

	const ISequencedStorage* storage(StorageType type) const noexcept;
	ISequencedStorage* storage(StorageType type) noexcept;

	void initialize();

signals:
	void parsedPageAdded(int row, StorageType storageType);
	void parsedPageLinksToThisResourceChanged(LinksToThisResourceChanges changes);
	void beginClearData();
	void endClearData();

protected:
	virtual std::shared_ptr<ISequencedStorage> createSequencedStorage() const;

protected slots:
	void addParsedPage(ParsedPagePtr parsedPagePtr, StorageType type);
	void onDataCleared();

private:
	std::unordered_map<StorageType, std::shared_ptr<ISequencedStorage>> m_sequencedStorageMap;

	friend class UnorderedDataCollection;
};

}