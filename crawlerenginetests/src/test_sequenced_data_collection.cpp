#include "test_sequenced_data_collection.h"
#include "test_sequenced_storage.h"

namespace CrawlerEngineTests
{

TestSequencedDataCollection::TestSequencedDataCollection(const CrawlerEngine::UnorderedDataCollection* collection)
	: SequencedDataCollection(collection)
{
}

std::shared_ptr<CrawlerEngine::ISequencedStorage> TestSequencedDataCollection::createSequencedStorage() const
{
	return std::static_pointer_cast<CrawlerEngine::ISequencedStorage>(std::make_shared<CrawlerEngine::TestSequencedStorage>());
}

}