#include "background_renderer.h"
#include "iview_model.h"

namespace QuickieWebBot
{

BackgroundRenderer::BackgroundRenderer(const IViewModel* viewModel)
	: m_viewModel(viewModel)
{
}

void BackgroundRenderer::render(QPainter* painter, const QStyleOptionViewItem& option, const QModelIndex& index) const
{
	QColor bgColor = m_viewModel->hoveredIndex().row() == index.row() ?
		m_viewModel->hoveredBackgroundColor(index) :
		m_viewModel->backgroundColor(index);

	painter->fillRect(option.rect, bgColor);
}

void BackgroundRenderer::invalidateCacheIndex(const QModelIndex&)
{
}

void BackgroundRenderer::invalidateCache()
{
}

void BackgroundRenderer::setCacheSize(int)
{
}

}