#pragma once

#include "irenderer.h"
#include "iview_model.h"

namespace SeoSpider
{

class ItemRenderer : public IRenderer
{
public:
	ItemRenderer(const IViewModel* viewModel);

	virtual void draw(QPainter* painter, const QStyleOptionViewItem& option, const QModelIndex& index) const override;

	virtual void invalidateCacheIndex(const QModelIndex& index) const override;
	virtual void invalidateCache() const override;
	virtual void setCacheSize(int cacheSize) override;

	virtual void addRenderer(int rendererTypes) override;

private:
	const IViewModel* m_viewModel;
	QList<IRenderer*> m_renderers;
};

}