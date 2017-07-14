#pragma once

#include "grid_view_painter.h"

namespace QuickieWebBot
{

class GridViewPainterBackground : public GridViewPainter
{
public:
	GridViewPainterBackground(QColor color, QColor alternateColor);
	virtual void paint(QPainter* painter, const QRect& rect, const QModelIndex& index) const override;

private:
	QColor m_color;
	QColor m_alternateColor;
};

}