#include "grid_view_painter_background.h"

namespace QuickieWebBot
{

GridViewPainterBackground::GridViewPainterBackground()
{
}

void GridViewPainterBackground::paint(QPainter* painter, const QRect& rect, const QModelIndex& index) const
{
	if (index.row() % 2 == 0)
	{
		painter->fillRect(rect, QColor("#eeeeee"));
	}
}

}