#include "viewport_percent_resize_policy.h"
#include "table_view.h"

namespace QuickieWebBot
{

ViewportPercentResizePolicy::ViewportPercentResizePolicy(std::vector<int> columnsPercentSize)
	: m_columnsPercentSize(columnsPercentSize)
{
}

void ViewportPercentResizePolicy::resize(TableView* view) const noexcept
{
	for (int i = 0; i < m_columnsPercentSize.size(); ++i)
	{
		view->setColumnWidth(static_cast<int>(i), columnSize(i, view));
	}
}

void ViewportPercentResizePolicy::init(TableView*, const IResizePolicy*) const noexcept
{
}

int ViewportPercentResizePolicy::columnSize(int column, const TableView* view) const noexcept
{
	int s_extraWidth = 5; // view border, etc

	int scrollBarWidth = view->verticalScrollBar()->width();
	int width = view->width() - scrollBarWidth - s_extraWidth;

	assert(m_columnsPercentSize.size() > column);

	int columnWidth = m_columnsPercentSize[column] * width / 100;

	return columnWidth;
}

QObject* ViewportPercentResizePolicy::qobject() noexcept
{
	return this;
}

}