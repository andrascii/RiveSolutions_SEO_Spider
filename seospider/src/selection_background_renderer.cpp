#include "stdafx.h"
#include "selection_background_renderer.h"
#include "iview_model.h"
#include "model_helpers.h"

namespace SeoSpider
{

SelectionBackgroundRenderer::SelectionBackgroundRenderer(const IViewModel* viewModel)
	: m_viewModel(viewModel)
{
}

void SelectionBackgroundRenderer::draw(QPainter* painter, const QStyleOptionViewItem& option, const QModelIndex& index) const
{
	if (option.state & QStyle::State_Selected)
	{
		const QModelIndex underlyingIndex = getUnderlyingIndex(index);
		if (underlyingIndex.isValid() && underlyingIndex.row() < underlyingIndex.model()->rowCount())
		{
			painter->fillRect(option.rect, m_viewModel->selectedBackgroundColor(index));
		}
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

void SelectionBackgroundRenderer::addRenderer(int)
{
	ASSERT(!"Attempt to add renderer to leaf");
}

}