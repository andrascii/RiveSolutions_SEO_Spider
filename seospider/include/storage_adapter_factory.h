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
	IStorageAdapter* createParsedPageInfoStorage(StorageAdapterType type, CrawlerEngine::SequencedDataCollection* sequencedDataCollection) const;
	IStorageAdapter* createPageLinksStorage(PageLinkContext type, ParsedPageInfoPtr associatedPageRawInfoPointer);

	static QVector<ParsedPageInfo::Column> parsedPageAvailableColumns(StorageAdapterType type);

protected:
	static void setupAvailablePageLinkColumns(IPageLinksStorageAdapter* storageAdapter);
};

}
