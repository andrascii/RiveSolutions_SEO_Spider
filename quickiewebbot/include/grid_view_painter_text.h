#pragma once

#include "grid_view_painter.h"
#include <chrono>

namespace QuickieWebBot
{

class GridViewPainterText : public GridViewPainter
{
public:
	GridViewPainterText(int cacheSize = 0);
	virtual void paint(QPainter* painter, const QRect& rect, const QModelIndex& index) const override;

private:
	struct Cache
	{
		QPixmap pixmap;
		std::chrono::system_clock::time_point lastAccess;
	};
	using CacheKey = std::pair<QString, std::pair<int, int>>;

	QPixmap* getCached(const CacheKey& key) const;
	void removeExtraCache() const;

	void applyTextBold(QPainter* painter, int flags) const;
	int textAlign(int flags) const;
	QRect paintDecorator(QPainter* painter, const QModelIndex& index, const QRect& rect) const;

private:
	int m_marginLeft;
	int m_marginTop;
	int m_marginRight;
	int m_marginBottom;

	mutable std::map<CacheKey, Cache> m_cache;
	mutable std::multimap<std::chrono::system_clock::time_point, CacheKey> m_cacheAccessTime;
	int m_cacheSize;
};

}