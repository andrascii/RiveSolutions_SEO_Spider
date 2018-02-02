#pragma once

#include "sequenced_data_collection.h"

namespace CrawlerEngine
{

class UnorderedDataCollection;

}

namespace CrawlerEngineTests
{

class TestSequencedDataCollection : public CrawlerEngine::SequencedDataCollection
{
public:
	TestSequencedDataCollection(const CrawlerEngine::UnorderedDataCollection* collection);

protected:
	friend class TestsCrawler;

	virtual std::shared_ptr<CrawlerEngine::ISequencedStorage> createSequencedStorage() const;
};

}