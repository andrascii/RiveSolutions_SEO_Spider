#pragma once

#include "igrid_view_painter.h"

namespace QuickieWebBot
{

class GridViewPainterBackground : public IGridViewPainter
{
public:
	GridViewPainterBackground(QColor color, QColor alternateColor);
	virtual void paint(QPainter* painter, const QStyleOptionViewItem& option, const QModelIndex& index) const override;

private:
	QColor m_color;
	QColor m_alternateColor;
};

}