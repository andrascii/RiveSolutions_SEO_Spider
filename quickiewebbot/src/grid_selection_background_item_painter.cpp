#include "grid_selection_background_item_painter.h"

namespace QuickieWebBot
{

GridSelectionBackgroundItemPainter::GridSelectionBackgroundItemPainter(QColor color, QColor alternateColor)
	: GridViewPainterBackground(color, alternateColor)
{
}

void GridSelectionBackgroundItemPainter::paint(QPainter* painter, const QStyleOptionViewItem& option, const QModelIndex& index) const
{
	if (option.state & QStyle::State_Selected)
	{
		GridViewPainterBackground::paint(painter, option, index);
	}
}

}