#include "filter_info_factory.h"
#include "storage_adapter_type.h"

namespace SeoSpider
{

namespace
{
std::map<StorageAdapterType, FilterInfo> filters
{
	{
		StorageAdapterType::StorageAdapterTypeNonAsciiLinks,
		{
			QObject::tr("Upper Case Title"),
			QObject::tr("Upper Case Description")
		}
	}
};
}

std::optional<FilterInfo> FilterInfoFactory::filterInfo(StorageAdapterType filter)
{
	auto it = filters.find(filter);
	if (it != filters.end())
	{
		return it->second;
	}

	return std::optional<FilterInfo>();
}

}
