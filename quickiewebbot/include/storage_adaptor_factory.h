#pragma once

#include "data_collection.h"
#include "summary_category_item.h"

namespace QuickieWebBot
{

class StorageAdaptor;

class StorageAdaptorFactory
{
public:
	StorageAdaptorFactory(WebCrawler::DataCollection* dataCollection);

	StorageAdaptor* create(SummaryCategoryItem type);

protected:
	void setupAvailableColumns(StorageAdaptor* storageAdaptor, SummaryCategoryItem type) const;

private:
	WebCrawler::DataCollection* m_dataCollection;
	std::map<SummaryCategoryItem, StorageAdaptor*> m_storageAdaptors;
};

}
