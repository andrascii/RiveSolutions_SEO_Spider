#include "selection_background_renderer.h"
#include "iview_model.h"

namespace QuickieWebBot
{

SelectionBackgroundRenderer::SelectionBackgroundRenderer(const IViewModel* viewModel)
	: m_viewModel(viewModel)
{
}

void SelectionBackgroundRenderer::render(QPainter* painter, const QStyleOptionViewItem& option, const QModelIndex& index) const
{
	if (option.state & QStyle::State_Selected)
	{
		painter->fillRect(option.rect, m_viewModel->selectionBackgroundColor(index));
	}
}

void SelectionBackgroundRenderer::invalidateCacheIndex(const QModelIndex&) const
{
}

void SelectionBackgroundRenderer::invalidateCache() const
{
}

void SelectionBackgroundRenderer::setCacheSize(int)
{
}

}