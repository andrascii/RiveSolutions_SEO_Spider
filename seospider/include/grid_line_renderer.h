#pragma once

#include "irenderer.h"
#include "iview_model.h"

namespace SeoSpider
{

class GridLineRenderer : public IRenderer
{
public:
	GridLineRenderer(const IViewModel* viewModel);

	virtual void draw(QPainter* painter, const QStyleOptionViewItem& option, const QModelIndex& index) const override;

	virtual void invalidateCacheIndex(const QModelIndex& index) const override;
	virtual void invalidateCache() const override;
	virtual void setCacheSize(int cacheSize) override;

	virtual void addRenderer(int rendererTypes) override;

private:
	void renderThisIfItAboveHoveredHelper(QPainter* painter, const QRect& adjustedRect, const QModelIndex& index) const noexcept;
	void renderHelper(QPainter* painter, const QRect& adjustedRect, const QModelIndex& index) const noexcept;

	bool isRowSelected(int row) const noexcept;

private:
	const IViewModel* m_viewModel;
};

}