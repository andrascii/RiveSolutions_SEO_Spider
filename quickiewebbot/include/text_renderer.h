#pragma once

#include "irenderer.h"

namespace QuickieWebBot
{

class IGridViewModel;

class TextRenderer : public IRenderer
{
public:
	explicit TextRenderer(const IGridViewModel* viewModel, int maxCacheSize = 0);
	virtual void render(QPainter* painter, const QStyleOptionViewItem& option, const QModelIndex& index) const override;

private:
	QPixmap* cached(const QModelIndex& index) const;
	QRect paintDecorator(QPainter* painter, const QModelIndex& index, const QRect& rect) const;
	void clearCacheIfNeeded() const noexcept;

private:
	const IGridViewModel* m_viewModel;

	mutable std::map<QModelIndex, QPixmap> m_cache;
	int m_maxCacheSize;
};

}