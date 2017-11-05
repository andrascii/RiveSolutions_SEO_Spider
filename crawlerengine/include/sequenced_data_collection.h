#pragma once

#include "parsed_page.h"
#include "parsed_page_hasher_proxy.h"
#include "parsed_page_comparator.h"
#include "unordered_data_collection.h"
#include "storage_type.h"

namespace CrawlerEngine
{

class SequencedStorage
{
public:

	//
	// This wrapper returns only plain pointers
	//

	int size() const noexcept;
	void clear();

	void pushBack(const ParsedPagePtr& page);

	const ParsedPage* operator[](int idx) const noexcept;
	ParsedPage* operator[](int idx) noexcept;

	const ParsedPagePtr& p(int i) const;

	bool containsPointersWithUseCountGreaterThanOne() const noexcept;

private:
	QVector<ParsedPagePtr> m_pages;
};

class SequencedDataCollection : public QObject
{
	Q_OBJECT

public:
	SequencedDataCollection();

	const SequencedStorage* storage(StorageType type) const noexcept;

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

	SequencedStorage* storage(StorageType type) noexcept;

private:
	std::unordered_map<int, SequencedStorage> m_sequencedStorageMap;

	friend class UnorderedDataCollection;
};

}