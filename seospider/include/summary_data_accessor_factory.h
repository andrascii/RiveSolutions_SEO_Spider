#pragma once

namespace CrawlerEngine
{
class SequencedDataCollection;
}

namespace SeoSpider
{

class ISummaryDataAccessor;

class SummaryDataAccessorFactory
{
public:
	enum class DataAccessorType
	{
		DataAccessorTypeBegin,

		ErrorsFilterPage,
		GroupedErrorFilterPage,
		AllResourcesPage,

		DataAccessorTypeEnd
	};

	ISummaryDataAccessor* create(DataAccessorType accessorType, CrawlerEngine::SequencedDataCollection* guiStorage) const;
};

Q_DECLARE_METATYPE(SummaryDataAccessorFactory::DataAccessorType)

}
