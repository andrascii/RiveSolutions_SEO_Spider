#pragma once

#include "unordered_data_collection.h"
#include "storage_adapter_type.h"
#include "parsed_page_info.h"
#include "page_links_storage_adapter.h"

namespace SeoSpider
{

class IStorageAdapter;

class StorageAdapterFactory
{
public:
	IStorageAdapter* createParsedPageInfoStorage(StorageAdapterType type, const CrawlerEngine::SequencedDataCollection* sequencedDataCollection) const;
	IStorageAdapter* createPageLinksStorage(PageLinkContext type, ParsedPageInfoPtr associatedPageRawInfoPointer);

protected:
	static void setupAvailableColumns(IParsedPageStorageAdapter* storageAdapter, StorageAdapterType type);
	static void setupAvailablePageLinkColumns(IPageLinksStorageAdapter* storageAdapter);
};

}
