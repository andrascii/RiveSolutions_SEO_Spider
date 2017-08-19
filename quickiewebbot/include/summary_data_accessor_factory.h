#pragma once

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

	ISummaryDataAccessor* create(DataAccessorType accessorType) const noexcept;
};

}
