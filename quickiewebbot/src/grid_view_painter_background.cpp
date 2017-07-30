#include "grid_view_painter_background.h"

namespace QuickieWebBot
{

GridViewPainterBackground::GridViewPainterBackground(QColor color, QColor alternateColor)
	: m_color(color)
	, m_alternateColor(alternateColor)
{
}

void GridViewPainterBackground::paint(QPainter* painter, const QStyleOptionViewItem& option, const QModelIndex& index) const
{
	painter->fillRect(option.rect, index.row() % 2 == 0 ? m_color : m_alternateColor);
}

}