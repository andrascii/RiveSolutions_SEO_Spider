#include "grid_view_resize_policy.h"
#include "grid_view.h"

namespace QuickieWebBot
{

GridViewResizePolicy::GridViewResizePolicy()
{
}

void GridViewResizePolicy::resize(GridView*) const noexcept
{
}

void GridViewResizePolicy::init(GridView* gridView, const IGridViewResizePolicy* prev) const noexcept
{
	for (auto it = m_columnsSize.begin(); it != m_columnsSize.end(); ++it)
	{
		gridView->setColumnWidth(it->first, it->second);
	}
}

int GridViewResizePolicy::columnSize(int column, const GridView* gridView) const noexcept
{
	auto it = m_columnsSize.find(column);
	assert(it != m_columnsSize.end());

	return it->second;
}

void GridViewResizePolicy::setColumnsSize(std::map<int, int> size)
{
	m_columnsSize = size;
}

}