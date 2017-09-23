#pragma once

#include "parsed_page.h"
#include "parsed_page_hasher_proxy.h"
#include "parsed_page_comparator.h"
#include "storage_type.h"

namespace WebCrawler
{

class SequencedDataCollection;

class UnorderedDataCollection : public QObject
{
	Q_OBJECT

protected:
	using UnorderedStorageType = std::unordered_multiset<ParsedPagePtr, ParsedPageHasherProxy, ParsedPageComparatorProxy>;
	using UnorderedStorageTypePtr = std::shared_ptr<UnorderedStorageType>;

public:
	UnorderedDataCollection(QObject* parent);
	~UnorderedDataCollection();

	bool isParsedPageExists(const ParsedPagePtr& parsedPagePtr, StorageType type) const noexcept;
	void addParsedPage(const ParsedPagePtr& parsedPagePtr, StorageType type) noexcept;
	ParsedPagePtr removeParsedPage(const ParsedPagePtr& parsedPagePtr, StorageType type) noexcept;
	const ParsedPagePtr parsedPage(const ParsedPagePtr& parsedPagePtr, StorageType type) const noexcept;

	SequencedDataCollection* sequencedDataCollection() const noexcept;

	Q_SIGNAL void parsedPageAdded(ParsedPagePtr parsedPagePtr, int type);

protected:
	UnorderedStorageTypePtr& storage(StorageType type) noexcept;
	const UnorderedStorageTypePtr& storage(StorageType type) const noexcept;
	
private:
	void checkStorageType(StorageType type) const noexcept;
	void initializeStorages();

private:
	std::unordered_map<int, UnorderedStorageTypePtr> m_unorderedStorageMap;
	SequencedDataCollection* m_sequencedDataCollection;
};

}
