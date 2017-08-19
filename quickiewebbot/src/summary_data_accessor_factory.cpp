#include "summary_data_accessor_factory.h"
#include "summary_filter_data_accessor.h"

namespace QuickieWebBot
{

ISummaryDataAccessor* SummaryDataAccessorFactory::create(DataAccessorType accessorType) const noexcept
{
	assert(accessorType > DataAccessorType::DataAccessorTypeBegin &&
		accessorType < DataAccessorType::DataAccessorTypeEnd);

	switch (accessorType)
	{
		case DataAccessorType::ErrorsFilterPage:
		{
			return new SummaryFilterDataAccessor;
		}
	}

	return nullptr;
}

}