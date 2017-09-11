#pragma once

#include "irenderer.h"

namespace QuickieWebBot
{

class IViewModel;

class BackgroundRenderer : public IRenderer
{
public:
	BackgroundRenderer(const IViewModel* viewModel, int cacheSize = 0);

	virtual void draw(QPainter* painter, const QStyleOptionViewItem& option, const QModelIndex& index) const override;

	virtual void invalidateCacheIndex(const QModelIndex& index) const override;
	virtual void invalidateCache() const override;
	virtual void setCacheSize(int cacheSize) override;

	virtual void addRenderer(int rendererTypes) override;

private:
	void clearCacheIfNeeded() const;
	QPixmap* cached(const QModelIndex& index) const noexcept;

private:
	const IViewModel* m_viewModel;

	mutable std::map<QModelIndex, QPixmap> m_cache;

	int m_cacheSize;
};

}