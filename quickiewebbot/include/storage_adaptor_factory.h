#pragma once

#include "unordered_data_collection.h"
#include "storage_adaptor_type.h"
#include "parsed_page_info.h"
#include "page_links_storage_adaptor.h"

namespace QuickieWebBot
{

class IStorageAdaptor;

class StorageAdaptorFactory
{
public:
	IStorageAdaptor* createPageRawInfoStorage(StorageAdaptorType type, WebCrawler::SequencedDataCollection* guiStorage) const;
	IStorageAdaptor* createPageLinksStorage(PageLinkContext type, ParsedPageInfoPtr associatedPageRawInfoPointer);

protected:
	static void setupAvailableColumns(IParsedPageStorageAdaptor* storageAdaptor, StorageAdaptorType type);
	static void setupAvailablePageLinkColumns(IPageLinksStorageAdaptor* storageAdaptor);
};

}
