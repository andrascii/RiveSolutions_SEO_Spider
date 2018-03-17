#pragma once

namespace SeoSpider
{
enum class StorageAdapterType;

struct FilterInfo
{
	QString title;
	QString description;
	// WarningLevel
};

class FilterInfoFactory
{
public:
	std::optional<FilterInfo> filterInfo(StorageAdapterType filter);

};

}
