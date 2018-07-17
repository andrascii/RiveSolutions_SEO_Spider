#include "background_renderer.h"
#include "iview_model.h"
#include "model_helpers.h"

namespace SeoSpider
{

BackgroundRenderer::BackgroundRenderer(const IViewModel* viewModel, int cacheSize)
	: m_viewModel(viewModel)
	, m_cache(cacheSize)
{
	ASSERT(cacheSize > 0);
}

void BackgroundRenderer::draw(QPainter* painter, const QStyleOptionViewItem& option, const QModelIndex& index) const
{
	const QModelIndex underlyingIndex = getUnderlyingIndex(index);

	QPixmap* pixmap = m_cache.object(underlyingIndex);

	if (pixmap && pixmap->rect().size() == option.rect.size())
	{
		painter->drawPixmap(option.rect, *pixmap);
		return;
	}

	const QColor bgColor = getUnderlyingIndex(m_viewModel->hoveredIndex()).row() == underlyingIndex.row() ?
		m_viewModel->hoveredBackgroundColor(index) :
		m_viewModel->backgroundColor(index);

	pixmap = new QPixmap(option.rect.size());
	pixmap->fill(bgColor);

	painter->drawPixmap(option.rect, *pixmap);

	m_cache.insert(underlyingIndex, pixmap);
}

void BackgroundRenderer::invalidateCacheIndex(const QModelIndex& index) const
{
	m_cache.remove(getUnderlyingIndex(index));
}

void BackgroundRenderer::invalidateCache() const
{
	m_cache.clear();
}

void BackgroundRenderer::setCacheSize(int cacheSize)
{
	ASSERT(cacheSize > 0);
	m_cache.setMaxCost(cacheSize);
}

void BackgroundRenderer::addRenderer(int)
{
	ASSERT(!"Attempt to add renderer to leaf");
}

}
