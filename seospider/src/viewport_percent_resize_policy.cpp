#include "viewport_percent_resize_policy.h"
#include "table_view.h"

namespace SeoSpider
{

ViewportPercentResizePolicy::ViewportPercentResizePolicy(QVector<int> columnsPercentSize)
	: m_columnsPercentSize(columnsPercentSize)
{
}

void ViewportPercentResizePolicy::resize(TableView* view) const noexcept
{
	for (int i = 0; i < m_columnsPercentSize.size(); ++i)
	{
		view->horizontalHeader()->resizeSection(i, columnSize(i, view));
	}
}

void ViewportPercentResizePolicy::init(TableView*, const IResizePolicy*) const noexcept
{
}

int ViewportPercentResizePolicy::columnSize(int column, const TableView* view) const noexcept
{
	const int s_extraWidth = 0; // view border, etc

	const int scrollBarWidth = view->verticalScrollBar()->isVisible() ? view->verticalScrollBar()->width() : 0;
	const int width = view->horizontalHeader()->width() - scrollBarWidth - s_extraWidth;

	DEBUG_ASSERT(m_columnsPercentSize.size() > column);

	const int columnWidth = m_columnsPercentSize[column] * width / 100.0;

	return columnWidth;
}

QObject* ViewportPercentResizePolicy::qobject() noexcept
{
	return this;
}

}