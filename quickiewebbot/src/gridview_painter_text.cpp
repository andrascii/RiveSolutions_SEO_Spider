#include "gridview_painter_text.h"

namespace QuickieWebBot
{

void GridViewPainterText::paint(QPainter* painter, const QRect& rect, const QModelIndex& index) const
{
	// TODO: optimize
	painter->drawText(rect, Qt::AlignLeft | Qt::AlignTop, index.data(Qt::DisplayRole).toString());
}

}