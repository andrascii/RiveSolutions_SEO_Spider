#pragma once

#include "parsed_page.h"
#include "parsed_page_hasher_proxy.h"
#include "parsed_page_comparator.h"
#include "unordered_data_collection.h"
#include "storage_type.h"

namespace CrawlerEngine
{

class SequencedDataCollection : public QObject
{
	Q_OBJECT

public:
	using SequencedStorageType = QVector<ParsedPagePtr>;
	using SequencedStorageTypePtr = std::shared_ptr<SequencedStorageType>;

	SequencedDataCollection();

	const SequencedStorageTypePtr& storage(StorageType type) const noexcept;

	SequencedStorageTypePtr& storage(StorageType type) noexcept;

signals:
	void parsedPageAdded(int row, int storageType);

	void parsedPageLinksToThisResourceChanged(LinksToThisResourceChanges changes);

	void beginClearData();
	void endClearData();

private slots:
	void addParsedPage(ParsedPagePtr parsedPagePtr, int type);

	void onDataCleared();

private:
	void initializeStorages();

private:
	std::unordered_map<int, SequencedStorageTypePtr> m_sequencedStorageMap;

	friend class UnorderedDataCollection;
};

}