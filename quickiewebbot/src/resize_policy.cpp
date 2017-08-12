#include "resize_policy.h"
#include "table_view.h"

namespace QuickieWebBot
{

void ResizePolicy::resize(TableView*) const noexcept
{
}

void ResizePolicy::init(TableView* view, const IResizePolicy* prev) const noexcept
{
	for (auto it = m_columnsSize.begin(); it != m_columnsSize.end(); ++it)
	{
		view->setColumnWidth(it->first, it->second);
	}
}

int ResizePolicy::columnSize(int column, const TableView* view) const noexcept
{
	auto it = m_columnsSize.find(column);
	assert(it != m_columnsSize.end());

	return it->second;
}

void ResizePolicy::setColumnsSize(std::map<int, int> size)
{
	m_columnsSize = size;
}

}