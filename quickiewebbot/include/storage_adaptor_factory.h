#pragma once

#include "data_collection.h"

namespace QuickieWebBot
{

class StorageAdaptor;

class StorageAdaptorFactory
{
public:
	StorageAdaptorFactory(DataCollection* dataCollection);

	StorageAdaptor* create(DataCollection::StorageType type) const;

private:
	DataCollection* m_dataCollection;
};

}
