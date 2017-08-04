#include "background_renderer.h"

namespace QuickieWebBot
{

void BackgroundRenderer::render(QPainter* painter, const QStyleOptionViewItem& option, const QModelIndex& index) const
{
	painter->fillRect(option.rect, index.data(Qt::BackgroundColorRole).value<QColor>());
}

}