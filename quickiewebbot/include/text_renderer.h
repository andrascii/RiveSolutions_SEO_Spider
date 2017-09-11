#pragma once

#include "irenderer.h"

namespace QuickieWebBot
{

class IViewModel;

class TextRenderer : public IRenderer
{
public:
	explicit TextRenderer(const IViewModel* viewModel, int cacheSize = 0);

	virtual void draw(QPainter* painter, const QStyleOptionViewItem& option, const QModelIndex& index) const override;

	virtual void invalidateCacheIndex(const QModelIndex& index) const override;
	virtual void invalidateCache() const override;
	virtual void setCacheSize(int cacheSize) override;

	virtual void addRenderer(int rendererTypes) override;

protected:
	virtual QString elidedText(const QString& string, const int width, bool isDecorationValid) const noexcept;
	virtual void paintDecorator(QPainter* painter, const QModelIndex& index, const QRect& rect) const;

	QPixmap* cached(const QModelIndex& index) const;
	void clearCacheIfNeeded() const noexcept;

	const IViewModel* viewModel() const noexcept;

private:
	const IViewModel* m_viewModel;

	mutable std::map<QModelIndex, QPixmap> m_cache;

	int m_cacheSize;
};

}