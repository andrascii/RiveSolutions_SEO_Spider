#pragma once

#include "grid_view_painter.h"

namespace QuickieWebBot
{

class GridViewPainterBackground : public GridViewPainter
{
public:
	GridViewPainterBackground();
	virtual void paint(QPainter* painter, const QRect& rect, const QModelIndex& index) const override;
};

}