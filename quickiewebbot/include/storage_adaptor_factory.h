#pragma once

#include "data_collection.h"
#include "storage_adaptor_type.h"
#include "page_raw_info.h"
#include "page_links_storage_adaptor.h"

namespace QuickieWebBot
{

class IStorageAdaptor;

class StorageAdaptorFactory
{
public:
	IStorageAdaptor* createPageRawInfoStorage(StorageAdaptorType type, WebCrawler::GuiStorage* guiStorage);
	IStorageAdaptor* createPageLinksStorage(PageLinkType type, PageRawInfoPtr associatedPageRawInfoPointer);

protected:
	void setupAvailableColumns(IStorageAdaptor* storageAdaptor, StorageAdaptorType type) const;
	void setupAvailablePageLinkColumns(IStorageAdaptor* storageAdaptor);
};

}
