#pragma once

#include "data_collection.h"

namespace QuickieWebBot
{

class StorageAdaptor;

class StorageAdaptorFactory
{
public:
	StorageAdaptorFactory(DataCollection* dataCollection);

	StorageAdaptor* create(DataCollection::StorageType type);

protected:
	void setupAvailableColumns(StorageAdaptor* storageAdaptor, DataCollection::StorageType type) const;

private:
	DataCollection* m_dataCollection;
	std::map<DataCollection::StorageType, StorageAdaptor*> m_storageAdaptors;
};

}
