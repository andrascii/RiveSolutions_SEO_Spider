#pragma once

#include "data_collection.h"
#include "summary_category_item.h"

namespace QuickieWebBot
{

class IStorageAdaptor;

class StorageAdaptorFactory
{
public:
	StorageAdaptorFactory(WebCrawler::GuiStorage* guiStorage);

	IStorageAdaptor* create(SummaryCategoryItem type);

protected:
	void setupAvailableColumns(IStorageAdaptor* storageAdaptor, SummaryCategoryItem type) const;

private:
	WebCrawler::GuiStorage* m_guiStorage;
	std::map<SummaryCategoryItem, IStorageAdaptor*> m_storageAdaptors;
};

}
