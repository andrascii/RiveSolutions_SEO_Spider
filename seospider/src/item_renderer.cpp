#include "item_renderer.h"
#include "text_renderer.h"
#include "background_renderer.h"
#include "selection_background_renderer.h"

namespace SeoSpider
{

ItemRenderer::ItemRenderer(const IViewModel* viewModel)
	: m_viewModel(viewModel)
{
}

void ItemRenderer::draw(QPainter* painter, const QStyleOptionViewItem& option, const QModelIndex& index) const
{
	std::for_each(m_renderers.begin(), m_renderers.end(), [&](IRenderer* renderer) { renderer->draw(painter, option, index); });
}

void ItemRenderer::invalidateCacheIndex(const QModelIndex& index) const
{
	std::for_each(m_renderers.begin(), m_renderers.end(), [&](IRenderer* renderer) { renderer->invalidateCacheIndex(index); });
}

void ItemRenderer::invalidateCache() const
{
	std::for_each(m_renderers.begin(), m_renderers.end(), [](IRenderer* renderer) { renderer->invalidateCache(); });
}

void ItemRenderer::setCacheSize(int cacheSize)
{
	std::for_each(m_renderers.begin(), m_renderers.end(), [cacheSize](IRenderer* renderer) { renderer->setCacheSize(cacheSize); });
}

void ItemRenderer::addRenderer(int rendererTypes)
{
	if (rendererTypes & BackgroundRendererType)
	{
		m_renderers.append(new BackgroundRenderer(m_viewModel, 2000));
	}

	if (rendererTypes & SelectionBackgroundRendererType)
	{
		m_renderers.append(new SelectionBackgroundRenderer(m_viewModel));
	}

	if (rendererTypes & PlainTextRendererType)
	{
		m_renderers.append(new TextRenderer(m_viewModel, 5000));
	}
}

}