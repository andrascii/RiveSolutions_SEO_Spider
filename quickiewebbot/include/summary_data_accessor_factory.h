#pragma once

namespace WebCrawler
{
	class DataCollection;
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

	ISummaryDataAccessor* create(DataAccessorType accessorType, WebCrawler::DataCollection* dataCollection) const noexcept;
};

}
