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

// 		if (flags && IModelDataAccessor::ItemFlagTextDecorator)
// 		{
// 			adjustedRect = paintDecorator(painter, index, adjustedRect);
// 		}
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

// 		if (flags && IModelDataAccessor::ItemFlagTextDecorator)
// 		{
// 			pixmapRect = paintDecorator(&painterPixmap, index, pixmapRect);
// 		}

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
		auto rangeIt = m_cacheAccessTime.equal_range(it->second.lastAccess);
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

		it->second.lastAccess = std::chrono::system_clock::now();
		m_cacheAccessTime.insert(std::make_pair(it->second.lastAccess, key));

		return &(it->second.pixmap);
	}

	return nullptr;
}

void GridViewPainterText::removeExtraCache() const
{
	int i = 0;

	for (auto it = std::begin(m_cacheAccessTime); i < m_cacheSize; i++)
	{
		m_cache.erase(it->second);
		it = m_cacheAccessTime.erase(it);
	}
}

void GridViewPainterText::applyTextBold(QPainter* painter, int flags) const
{
	//if (flags & IModelDataAccessor::ItemFlagTextBold)
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

// 	if (flags & IModelDataAccessor::ItemFlagAlignRight)
// 	{
// 		result |= Qt::AlignRight;
// 	}
// 	else if (flags & IModelDataAccessor::ItemFlagAlignCenter)
// 	{
// 		result |= Qt::AlignHCenter;
// 	}
// 	else
// 	{
// 		result |= Qt::AlignLeft;
// 	}

	return result;
}

QRect GridViewPainterText::paintDecorator(QPainter* painter, const QModelIndex& index, const QRect& rect) const
{
	QPixmap pixmap = qvariant_cast<QPixmap>(index.data(Qt::DecorationRole));
	QSize pixmapSize = pixmap.size();
	painter->drawPixmap(QRect(rect.topLeft(), pixmapSize), pixmap);

	return rect.adjusted(pixmapSize.width() + 5, 0, 0, 0);
}

}