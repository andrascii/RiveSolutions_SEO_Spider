#pragma once

#include "irenderer.h"

namespace QuickieWebBot
{

class IViewModel;

class TextRenderer : public IRenderer
{
public:
	explicit TextRenderer(int cacheSize = 0);

	virtual void render(QPainter* painter, const QStyleOptionViewItem& option, const QModelIndex& index) const override;
	virtual void resetCache() override;
	virtual void setCacheSize(int cacheSize) override;

protected:
	virtual QRect paintDecorator(QPainter* painter, const QModelIndex& index, const QRect& rect) const;

	QPixmap* cached(const QModelIndex& index) const;
	void clearCacheIfNeeded() const noexcept;

private:
	mutable std::map<QModelIndex, QPixmap> m_cache;
	int m_cacheSize;
};

}