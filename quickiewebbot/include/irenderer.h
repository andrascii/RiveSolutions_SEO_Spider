#pragma once


namespace QuickieWebBot
{

class IRenderer
{
public:
	virtual ~IRenderer() = default;
	virtual void render(QPainter* painter, const QStyleOptionViewItem& option, const QModelIndex& index) const = 0;

	virtual void invalidateCacheIndex(const QModelIndex& index) = 0;
	virtual void invalidateCache() = 0;
	virtual void setCacheSize(int cacheSize) = 0;
};

}