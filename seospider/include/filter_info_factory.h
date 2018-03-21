#pragma once

namespace SeoSpider
{

enum class StorageAdapterType;

struct FilterInfo
{
	QString title;
	QString description;
	QList<QByteArray> props;
	// WarningLevel
};

class FilterInfoFactory
{
public:
	std::optional<FilterInfo> filterInfo(StorageAdapterType filter);

};

}
