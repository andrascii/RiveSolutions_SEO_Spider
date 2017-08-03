#include "selection_background_renderer.h"

namespace QuickieWebBot
{

SelectionBackgroundRenderer::SelectionBackgroundRenderer(QColor color, QColor alternateColor)
	: BackgroundRenderer(color, alternateColor)
{
}

void SelectionBackgroundRenderer::render(QPainter* painter, const QStyleOptionViewItem& option, const QModelIndex& index) const
{
	if (option.state & QStyle::State_Selected)
	{
		BackgroundRenderer::render(painter, option, index);
	}
}

}