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

private:
	int m_marginLeft;
	int m_marginTop;
	int m_marginRight;
	int m_marginBottom;

	mutable QHash<CacheKey, Cache> m_cache;
	int m_cacheSize;
};

}