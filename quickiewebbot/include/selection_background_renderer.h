#pragma once

#include "background_renderer.h"

namespace QuickieWebBot
{
	
class SelectionBackgroundRenderer : public IRenderer
{
public:
	SelectionBackgroundRenderer(const IViewModel* viewModel);

	virtual void render(QPainter* painter, const QStyleOptionViewItem& option, const QModelIndex& index) const override;

	virtual void invalidateCacheIndex(const QModelIndex& index) override;
	virtual void invalidateCache() override;
	virtual void setCacheSize(int cacheSize) override;

private:
	const IViewModel* m_viewModel;
};

}
