#include "background_renderer.h"
#include "iview_model.h"

namespace SeoSpider
{

BackgroundRenderer::BackgroundRenderer(const IViewModel* viewModel, int cacheSize)
	: m_viewModel(viewModel)
	, m_cacheSize(cacheSize)
{
}

void BackgroundRenderer::draw(QPainter* painter, const QStyleOptionViewItem& option, const QModelIndex& index) const
{
	QPixmap* cachedPixmap = cached(index);

	if (cachedPixmap && cachedPixmap->rect().size() == option.rect.size())
	{
		painter->drawPixmap(option.rect, m_cache[index]);
		return;
	}

	const QColor bgColor = m_viewModel->hoveredIndex().row() == index.row() ?
		m_viewModel->hoveredBackgroundColor(index) :
		m_viewModel->backgroundColor(index);

	QPixmap pixmap(option.rect.size());
	pixmap.fill(bgColor);

	painter->drawPixmap(option.rect, pixmap);

	// m_cache[index] = pixmap;
	clearCacheIfNeeded();
}

void BackgroundRenderer::invalidateCacheIndex(const QModelIndex& index) const
{
	m_cache.erase(index);
}

void BackgroundRenderer::invalidateCache() const
{
	m_cache.clear();
}

void BackgroundRenderer::setCacheSize(int cacheSize)
{
	m_cacheSize = cacheSize;
}

void BackgroundRenderer::addRenderer(int)
{
	ASSERT(!"Attempt to add renderer to leaf");
}

void BackgroundRenderer::clearCacheIfNeeded() const
{
	if (m_cache.size() >= static_cast<size_t>(m_cacheSize))
	{
		invalidateCache();
	}
}

QPixmap* BackgroundRenderer::cached(const QModelIndex& index) const noexcept
{
	auto cachedPixmapIterator = m_cache.find(index);

	if (cachedPixmapIterator != std::end(m_cache))
	{
		return &cachedPixmapIterator->second;
	}

	return nullptr;
}

}