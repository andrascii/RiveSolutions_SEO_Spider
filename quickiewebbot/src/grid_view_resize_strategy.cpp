#include "grid_view_resize_strategy.h"
#include "grid_view.h"

namespace QuickieWebBot
{
GridViewResizeStrategy::GridViewResizeStrategy()
{
}

void GridViewResizeStrategy::resize(GridView* gridView) const
{
	Q_UNUSED(gridView);
}

void GridViewResizeStrategy::init(GridView* gridView, const IGridViewResizeStrategy* prev)
{
	for (auto it = m_columnsSize.begin(); it != m_columnsSize.end(); ++it)
	{
		gridView->setColumnWidth(it->first, it->second);
	}
}

int GridViewResizeStrategy::columnSize(int column, const GridView* gridView) const
{
	auto it = m_columnsSize.find(column);
	assert(it != m_columnsSize.end());

	return it->second;
}

void GridViewResizeStrategy::setColumnsSize(std::map<int, int> size)
{
	m_columnsSize = size;
}

}