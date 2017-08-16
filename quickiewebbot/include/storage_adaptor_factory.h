#pragma once

#include "data_collection.h"

namespace QuickieWebBot
{

class StorageAdaptor;

class StorageAdaptorFactory
{
public:
	StorageAdaptorFactory(WebCrawler::DataCollection* dataCollection);

	StorageAdaptor* create(WebCrawler::DataCollection::StorageType type);

protected:
	void setupAvailableColumns(StorageAdaptor* storageAdaptor, WebCrawler::DataCollection::StorageType type) const;

private:
	WebCrawler::DataCollection* m_dataCollection;
	std::map<WebCrawler::DataCollection::StorageType, StorageAdaptor*> m_storageAdaptors;
};

}
