#include "grid_view_painter_text.h"
#include "imodel_data_accessor.h"

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
	int flags = index.data(Qt::UserRole).toInt();

	QPainter* painterLocal = painter;
	if (m_cacheSize == 0)
	{
		
		painter->save();
		painter->setClipRect(adjustedRect);
		painter->setPen(qvariant_cast<QColor>(index.data(Qt::TextColorRole)));
		applyTextBold(painter, flags);

		painter->drawText(adjustedRect, textAlign(flags), index.data(Qt::DisplayRole).toString());

		painter->restore();
		return;
	}

	CacheKey key = std::make_pair(index.data(Qt::DisplayRole).toString(), std::make_pair(rect.width(), rect.height()));
	QPixmap* pixmapPointer = getCached(key);

	if (pixmapPointer == nullptr)
	{
		QRect pixmapRect(0, 0, adjustedRect.width(), adjustedRect.height());
	
		QPixmap pixmap(pixmapRect.size());
		pixmap.fill(Qt::transparent);
		QPainter painterPixmap(&pixmap);

		applyTextBold(&painterPixmap, flags);

		painterPixmap.setPen(qvariant_cast<QColor>(index.data(Qt::TextColorRole)));
		painterPixmap.drawText(pixmapRect, textAlign(flags), key.first);

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

	for (auto it = std::begin(m_cacheAccessTime); i < m_cacheSize; i++)
	{
		m_cache.remove(it->second);
		it = m_cacheAccessTime.erase(it);
	}
}

void GridViewPainterText::applyTextBold(QPainter* painter, int flags) const
{
	if (flags & IModelDataAccessor::ItemFlagTextBold)
	{
		QFont font = painter->font();
		font.setBold(true);
		font.setPointSize(font.pointSize() + 1);
		painter->setFont(font);
	}
}

int GridViewPainterText::textAlign(int flags) const
{
	int result = Qt::AlignTop;

	if (flags & IModelDataAccessor::ItemFlagAlignRight)
	{
		result |= Qt::AlignRight;
	}
	else if (flags & IModelDataAccessor::ItemFlagAlignCenter)
	{
		result |= Qt::AlignHCenter;
	}
	else
	{
		result |= Qt::AlignLeft;
	}

	return result;
}

}