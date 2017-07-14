#include "grid_view_painter_text.h"

namespace QuickieWebBot
{

GridViewPainterText::GridViewPainterText(int cacheSize)
	: m_cacheSize(cacheSize)
	, m_marginLeft(4)
	, m_marginTop(2)
	, m_marginRight(4)
	, m_marginBottom(2)
{
}

void GridViewPainterText::paint(QPainter* painter, const QRect& rect, const QModelIndex& index) const
{
	QRect adjustedRect = rect.adjusted(m_marginLeft, m_marginTop, -m_marginRight, -m_marginBottom);

	QPainter* painterLocal = painter;
	if (m_cacheSize == 0)
	{
		
		painter->save();
		painter->setClipRect(adjustedRect);
		painter->setPen(qvariant_cast<QColor>(index.data(Qt::TextColorRole)));

		painter->drawText(adjustedRect, Qt::AlignLeft | Qt::AlignTop, index.data(Qt::DisplayRole).toString());

		painter->restore();
		return;
	}

	// is not working now

	CacheKey key = std::make_pair(index.data(Qt::DisplayRole).toString(), std::make_pair(rect.width(), rect.height()));
	QPixmap* pixmapPointer = getCached(key);

	if (pixmapPointer == nullptr)
	{
		QRect pixmapRect(m_marginLeft, m_marginTop, rect.width() - m_marginLeft - m_marginRight, rect.height() - m_marginTop - m_marginBottom);
	
		QPixmap pixmap(pixmapRect.size());
		pixmap.fill(Qt::transparent);
		QPainter painterPixmap(&pixmap);

		painterPixmap.setPen(qvariant_cast<QColor>(index.data(Qt::TextColorRole)));
		painterPixmap.drawText(pixmapRect, Qt::AlignLeft | Qt::AlignTop, key.first);

		auto accessTime = std::chrono::system_clock::now();
		m_cache[key] = Cache{ pixmap, accessTime };
		m_cacheAccessTime.insert(std::make_pair(accessTime, key));

		pixmapPointer = &(m_cache[key].pixmap);
		
	}

	painter->drawPixmap(adjustedRect, *pixmapPointer);
	if (m_cache.size() > m_cacheSize * 2)
	{
		removeExtraCache();
	}
	
}

QPixmap* GridViewPainterText::getCached(const CacheKey& key) const
{
	auto it = m_cache.find(key);
	if (it != std::end(m_cache))
	{
		auto rangeIt = m_cacheAccessTime.equal_range(it->lastAccess);
		auto rmIt = rangeIt.first;
		for (auto rangeIt1 = rangeIt.first; rangeIt1 != rangeIt.second; rangeIt1++)
		{
			if (rangeIt1->second == key)
			{
				rmIt = rangeIt1;
				break;
			}
		}

		m_cacheAccessTime.erase(rmIt);

		it->lastAccess = std::chrono::system_clock::now();
		m_cacheAccessTime.insert(std::make_pair(it->lastAccess, key));

		return &(it->pixmap);
	}

	return nullptr;
}

void GridViewPainterText::removeExtraCache() const
{
	int i = 0;
	for (auto it = std::begin(m_cacheAccessTime); i < m_cacheSize / 2; i++)
	{
		m_cache.remove(it->second);
		it = m_cacheAccessTime.erase(it);

	}
}

}