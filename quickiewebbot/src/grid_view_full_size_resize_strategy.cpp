#include "grid_view_full_size_resize_strategy.h"
#include "grid_view.h"

namespace QuickieWebBot
{

GridViewFullSizeResizeStrategy::GridViewFullSizeResizeStrategy(std::vector<int> columnsPercentSize)
	: m_columnsPercentSize(columnsPercentSize)
{
}

void GridViewFullSizeResizeStrategy::resize(GridView* gridView) const
{
	for (int i = 0; i < m_columnsPercentSize.size(); ++i)
	{
		gridView->setColumnWidth(static_cast<int>(i), columnSize(i, gridView));
	}
}

void GridViewFullSizeResizeStrategy::init(GridView* gridView, const IGridViewResizeStrategy* prev)
{
	Q_UNUSED(gridView);
	Q_UNUSED(prev);
}

int GridViewFullSizeResizeStrategy::columnSize(int column, const GridView* gridView) const
{
	int s_extraWidth = 5; // gridwiew border, etc
	int scroolBarWidth = gridView->verticalScrollBar()->width();
	int width = gridView->width() - scroolBarWidth - s_extraWidth;
	assert(m_columnsPercentSize.size() > column);
	int columnWidth = m_columnsPercentSize[column] * width / 100;
	return columnWidth;
}

}