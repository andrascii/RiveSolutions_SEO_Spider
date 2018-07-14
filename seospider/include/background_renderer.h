#pragma once

#include "irenderer.h"

namespace SeoSpider
{

class IViewModel;

class BackgroundRenderer : public IRenderer
{
public:
	BackgroundRenderer(const IViewModel* viewModel, int cacheSize = 1000);

	virtual void draw(QPainter* painter, const QStyleOptionViewItem& option, const QModelIndex& index) const override;

	virtual void invalidateCacheIndex(const QModelIndex& index) const override;
	virtual void invalidateCache() const override;
	virtual void setCacheSize(int cacheSize) override;

	virtual void addRenderer(int rendererTypes) override;

private:
	const IViewModel* m_viewModel;

	mutable QCache<QModelIndex, QPixmap> m_cache;
};

}