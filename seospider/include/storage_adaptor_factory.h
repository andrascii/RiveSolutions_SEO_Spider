#pragma once

#include "unordered_data_collection.h"
#include "storage_adaptor_type.h"
#include "parsed_page_info.h"
#include "page_links_storage_adaptor.h"

namespace SeoSpider
{

class IStorageAdaptor;

class StorageAdaptorFactory
{
public:
	IStorageAdaptor* createPageRawInfoStorage(StorageAdaptorType type, CrawlerEngine::SequencedDataCollection* sequencedDataCollection) const;
	IStorageAdaptor* createPageLinksStorage(PageLinkContext type, ParsedPageInfoPtr associatedPageRawInfoPointer);

protected:
	static void setupAvailableColumns(IParsedPageStorageAdaptor* storageAdaptor, StorageAdaptorType type);
	static void setupAvailablePageLinkColumns(IPageLinksStorageAdaptor* storageAdaptor);
};

}
