#include "full_size_resize_policy.h"
#include "table_view.h"

namespace QuickieWebBot
{

FullSizeResizePolicy::FullSizeResizePolicy(std::vector<int> columnsPercentSize)
	: m_columnsPercentSize(columnsPercentSize)
{
}

void FullSizeResizePolicy::resize(TableView* gridView) const noexcept
{
	for (int i = 0; i < m_columnsPercentSize.size(); ++i)
	{
		gridView->setColumnWidth(static_cast<int>(i), columnSize(i, gridView));
	}
}

void FullSizeResizePolicy::init(TableView*, const IResizePolicy*) const noexcept
{
}

int FullSizeResizePolicy::columnSize(int column, const TableView* gridView) const noexcept
{
	int s_extraWidth = 5; // gridwiew border, etc

	int scrollBarWidth = gridView->verticalScrollBar()->width();
	int width = gridView->width() - scrollBarWidth - s_extraWidth;

	assert(m_columnsPercentSize.size() > column);

	int columnWidth = m_columnsPercentSize[column] * width / 100;

	return columnWidth;
}

}