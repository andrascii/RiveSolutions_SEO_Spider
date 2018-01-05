#include "default_column_resize_policy.h"
#include "table_view.h"

namespace SeoSpider
{

void DefaultColumnResizePolicy::resize(TableView*) const noexcept
{
}

QObject* DefaultColumnResizePolicy::qobject() noexcept
{
	return this;
}

void DefaultColumnResizePolicy::init(TableView* view, const IResizePolicy* prev) const noexcept
{
	Q_UNUSED(prev);

	for (auto it = m_columnsSize.begin(); it != m_columnsSize.end(); ++it)
	{
		view->setColumnWidth(it->first, it->second);
	}
}

int DefaultColumnResizePolicy::columnSize(int column, const TableView* view) const noexcept
{
	Q_UNUSED(view);

	auto it = m_columnsSize.find(column);
	DEBUG_ASSERT(it != m_columnsSize.end());

	return it->second;
}

void DefaultColumnResizePolicy::setColumnsSize(const std::map<int, int>& size)
{
	m_columnsSize = size;
}

}