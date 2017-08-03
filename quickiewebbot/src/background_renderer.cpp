#include "background_renderer.h"

namespace QuickieWebBot
{

BackgroundRenderer::BackgroundRenderer(QColor color, QColor alternateColor)
	: m_color(color)
	, m_alternateColor(alternateColor)
{
}

void BackgroundRenderer::render(QPainter* painter, const QStyleOptionViewItem& option, const QModelIndex& index) const
{
	painter->fillRect(option.rect, index.row() % 2 == 0 ? m_color : m_alternateColor);
}

}