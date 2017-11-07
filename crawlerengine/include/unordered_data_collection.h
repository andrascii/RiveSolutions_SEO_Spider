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

	ParsedPagePtr removeParsedPage(const ParsedPagePtr& parsedPagePtr, StorageType type) noexcept;

	const ParsedPagePtr parsedPage(const ParsedPagePtr& parsedPagePtr, StorageType type) const noexcept;


	SequencedDataCollection* createSequencedDataCollection(QThread* targetThread) const;

	void clearData();

signals:
	void parsedPageAdded(ParsedPagePtr parsedPagePtr, int type);
	
	void parsedPageLinksToThisResourceChanged(LinksToThisResourceChanges changes);

	void dataCleared();

protected:
	UnorderedStorageType& storage(StorageType type) noexcept;

	const UnorderedStorageType& storage(StorageType type) const noexcept;
	
private:
	void checkStorageType(StorageType type) const noexcept;

	void initializeStorages();

private:
	std::unordered_map<int, UnorderedStorageType> m_unorderedStorageMap;
};

}
