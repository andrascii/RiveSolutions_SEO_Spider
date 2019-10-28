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
	IStorageAdapter* createPageLinksStorage(PageLinkContext type, ParsedPageInfoPtr associatedPageRawInfoPointer, CrawlerEngine::SequencedDataCollection* sequencedDataCollection);

	static QVector<ParsedPageInfo::Column> parsedPageAllColumns();
	static QVector<ParsedPageInfo::Column> defaultColumns(StorageAdapterType type);

protected:
	static void setupAvailablePageLinkColumns(IPageLinksStorageAdapter* storageAdapter);
};

}
