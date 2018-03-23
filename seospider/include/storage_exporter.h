#pragma once

#include "data_collection_groups_factory.h"

namespace CrawlerEngine
{

class SequencedDataCollection;

}

namespace SeoSpider
{

class StorageExporter final
{
public:
	static void exportStorage(
		const CrawlerEngine::SequencedDataCollection* dataCollection, 
		const std::vector<DCStorageDescription>& storages
	);
};

}