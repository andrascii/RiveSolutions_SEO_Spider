#include "item_renderer.h"
#include "text_renderer.h"
#include "background_renderer.h"
#include "selection_background_renderer.h"
#include "grid_line_renderer.h"

namespace QuickieWebBot
{

ItemRenderer::ItemRenderer(const IViewModel* viewModel)
	: m_viewModel(viewModel)
{
}

void ItemRenderer::draw(QPainter* painter, const QStyleOptionViewItem& option, const QModelIndex& index) const
{
	for (IRenderer* renderer : m_renderers)
	{
		renderer->draw(painter, option, index);
	}
}

void ItemRenderer::invalidateCacheIndex(const QModelIndex& index) const
{
	for (IRenderer* renderer : m_renderers)
	{
		renderer->invalidateCacheIndex(index);
	}
}

void ItemRenderer::invalidateCache() const
{
	for (IRenderer* renderer : m_renderers)
	{
		renderer->invalidateCache();
	}
}

void ItemRenderer::setCacheSize(int cacheSize)
{
	for (IRenderer* renderer : m_renderers)
	{
		renderer->setCacheSize(cacheSize);
	}
}

void ItemRenderer::addRenderer(int rendererTypes)
{
	if (rendererTypes & BackgroundRendererType)
	{
		m_renderers.append(new BackgroundRenderer(m_viewModel));
	}

	if (rendererTypes & SelectionBackgroundRendererType)
	{
		m_renderers.append(new SelectionBackgroundRenderer(m_viewModel));
	}

	if (rendererTypes & GridLineRendererType)
	{
		m_renderers.append(new GridLineRenderer(m_viewModel));
	}

	if (rendererTypes & PlainTextRendererType)
	{
		m_renderers.append(new TextRenderer(m_viewModel));
	}
}

}