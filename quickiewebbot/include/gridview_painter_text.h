#pragma once

#include "gridview_painter.h"

namespace QuickieWebBot
{

class GridViewPainterText : public GridViewPainter
{
public:
	virtual void paint(QPainter* painter, const QRect& rect, const QModelIndex& index) const override;
};

}