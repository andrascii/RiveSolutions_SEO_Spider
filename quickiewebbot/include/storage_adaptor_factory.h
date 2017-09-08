#pragma once

#include "data_collection.h"
#include "summary_category_item.h"

namespace QuickieWebBot
{

class StorageAdaptor;

class StorageAdaptorFactory
{
public:
	StorageAdaptorFactory(WebCrawler::GuiStorage* guiStorage);

	StorageAdaptor* create(SummaryCategoryItem type);

protected:
	void setupAvailableColumns(StorageAdaptor* storageAdaptor, SummaryCategoryItem type) const;

private:
	WebCrawler::GuiStorage* m_guiStorage;
	std::map<SummaryCategoryItem, StorageAdaptor*> m_storageAdaptors;
};

}
