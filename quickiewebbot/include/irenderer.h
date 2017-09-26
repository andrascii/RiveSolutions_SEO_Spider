#pragma once


namespace QuickieWebBot
{

class IRenderer
{
public:
	enum RendererType
	{
		PlainTextRendererType			= 1 << 0,
		BackgroundRendererType			= 1 << 1,
		SelectionBackgroundRendererType	= 1 << 2,
		GridLineRendererType			= 1 << 3, // deprecated, use setShowAdditionalGrid of TableView instead
	};

	virtual ~IRenderer() = default;
	virtual void draw(QPainter* painter, const QStyleOptionViewItem& option, const QModelIndex& index) const = 0;

	virtual void invalidateCacheIndex(const QModelIndex& index) const = 0;
	virtual void invalidateCache() const = 0;
	virtual void setCacheSize(int cacheSize) = 0;

	virtual void addRenderer(int rendererTypes) = 0;
};

}