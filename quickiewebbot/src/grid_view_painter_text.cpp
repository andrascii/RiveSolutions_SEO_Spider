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
		QPainter painterPixmap(&pixmap);

		painterPixmap.setPen(qvariant_cast<QColor>(index.data(Qt::TextColorRole)));
		painterPixmap.fillRect(pixmapRect, Qt::transparent);
		painterPixmap.drawText(pixmapRect, Qt::AlignLeft | Qt::AlignTop, key.first);

		m_cache[key] = Cache{ pixmap, std::chrono::system_clock::now() };

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
		it->lastAccess = std::chrono::system_clock::now();
		return &(it->pixmap);
	}

	return nullptr;
}

void GridViewPainterText::removeExtraCache() const
{

}

}