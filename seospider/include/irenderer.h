#pragma once
#include "stdafx.h"


namespace SeoSpider
{

class IRenderer
{
public:
	enum RendererType
	{
		PlainTextRendererType			= 1 << 0,
		BackgroundRendererType			= 1 << 1,
		SelectionBackgroundRendererType	= 1 << 2
	};

	virtual ~IRenderer() = default;
	virtual void draw(QPainter* painter, const QStyleOptionViewItem& option, const QModelIndex& index) const = 0;

	virtual void invalidateCacheIndex(const QModelIndex& index) const = 0;
	virtual void invalidateCache() const = 0;
	virtual void setCacheSize(int cacheSize) = 0;

	virtual void addRenderer(int rendererTypes) = 0;
};

}