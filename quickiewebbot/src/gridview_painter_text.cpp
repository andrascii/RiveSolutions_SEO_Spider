#include "gridview_painter_text.h"

namespace QuickieWebBot
{

void GridViewPainterText::paint(QPainter* painter, const QRect& rect, const QModelIndex& index) const
{
	// TODO: optimize
	painter->save();
	
	painter->setPen(Qt::black);
	painter->drawText(rect, Qt::AlignLeft | Qt::AlignTop, index.data(Qt::DisplayRole).toString());
	
	painter->restore();
}

}