#include "selection_background_renderer.h"
#include "itable_model.h"

namespace QuickieWebBot
{

void SelectionBackgroundRenderer::render(QPainter* painter, const QStyleOptionViewItem& option, const QModelIndex& index) const
{
	if (option.state & QStyle::State_Selected)
	{
		painter->fillRect(option.rect, index.data(ITableModel::SelectionBackgroundColorRole).value<QColor>());
	}
}

void SelectionBackgroundRenderer::resetCache()
{
}

void SelectionBackgroundRenderer::setCacheSize(int)
{
}

}