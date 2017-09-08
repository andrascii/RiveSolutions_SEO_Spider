#pragma once

namespace WebCrawler
{
class GuiStorage;
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

	ISummaryDataAccessor* create(DataAccessorType accessorType, WebCrawler::GuiStorage* guiStorage) const noexcept;
};

}
