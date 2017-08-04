#include "selection_background_renderer.h"
#include "igrid_model.h"

namespace QuickieWebBot
{

void SelectionBackgroundRenderer::render(QPainter* painter, const QStyleOptionViewItem& option, const QModelIndex& index) const
{
	if (option.state & QStyle::State_Selected)
	{
		painter->fillRect(option.rect, index.data(IGridModel::SelectionBackgroundColorRole).value<QColor>());
	}
}

}