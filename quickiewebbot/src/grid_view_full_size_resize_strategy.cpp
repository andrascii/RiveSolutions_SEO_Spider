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
	static int s_extraWidth = 5; // gridwiew border, etc
	int width = gridView->width() - s_extraWidth;
	for (size_t i = 0; i < m_columnsPercentSize.size(); ++i)
	{
		int columnWidth = m_columnsPercentSize[i] * width / 100;
		gridView->setColumnWidth(static_cast<int>(i), columnWidth);
	}
}

}