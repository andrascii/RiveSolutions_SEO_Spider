#pragma once

namespace WebCrawler
{
class SequencedDataCollection;
}

namespace QuickieWebBot
{

class ISummaryDataAccessor;

class SummaryDataAccessorFactory
{
public:
	enum class DataAccessorType
	{
		DataAccessorTypeBegin,

		ErrorsFilterPage,
		AllResourcesPage,

		DataAccessorTypeEnd
	};

	ISummaryDataAccessor* create(DataAccessorType accessorType, WebCrawler::SequencedDataCollection* guiStorage) const;
};

}
