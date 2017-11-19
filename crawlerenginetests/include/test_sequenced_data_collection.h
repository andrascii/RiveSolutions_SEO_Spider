#pragma once

#include "sequenced_data_collection.h"

namespace CrawlerEngineTests
{

class TestSequencedDataCollection : public CrawlerEngine::SequencedDataCollection
{
protected:
	friend class TestsCrawler;

	virtual std::shared_ptr<CrawlerEngine::ISequencedStorage> createSequencedStorage() const;
};

}