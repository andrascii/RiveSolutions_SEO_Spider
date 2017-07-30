#pragma once

#include "grid_view_painter_background.h"

namespace QuickieWebBot
{
	
class GridSelectionBackgroundItemPainter : public GridViewPainterBackground
{
public:
	GridSelectionBackgroundItemPainter(QColor color, QColor alternateColor);
	virtual void paint(QPainter* painter, const QStyleOptionViewItem& option, const QModelIndex& index) const override;
};

}
