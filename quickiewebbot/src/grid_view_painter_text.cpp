#include "grid_view_painter_text.h"
#include "igrid_data_accessor.h"
#include "quickie_web_bot_helpers.h"

namespace QuickieWebBot
{

GridViewPainterText::GridViewPainterText(int cacheSize)
	: m_cacheSize(cacheSize)
	, m_marginLeft(QuickieWebBotHelpers::pointsToPixels(4))
	, m_marginTop(QuickieWebBotHelpers::pointsToPixels(2))
	, m_marginRight(QuickieWebBotHelpers::pointsToPixels(4))
	, m_marginBottom(QuickieWebBotHelpers::pointsToPixels(2))
{
}

void GridViewPainterText::paint(QPainter* painter, const QRect& rect, const QModelIndex& index) const
{
	const bool isTextBold = index.data(Qt::FontRole).value<QFont>().bold();
	const bool isDecorationValid = index.data(Qt::DecorationRole).isValid();

	const int textAlignmentFlags = index.data(Qt::TextAlignmentRole).toInt();
	const QString paintingText = index.data(Qt::DisplayRole).toString();
	const QColor textColor = qvariant_cast<QColor>(index.data(Qt::TextColorRole));

	QRect adjustedRect = rect.adjusted(m_marginLeft, m_marginTop, -m_marginRight, -m_marginBottom);

	if (!m_cacheSize)
	{
		painter->save();
		painter->setClipRect(adjustedRect);
		painter->setPen(textColor);
		
		if (isTextBold)
		{
			applyTextBold(painter);
		}

		if (isDecorationValid)
		{
			adjustedRect = paintDecorator(painter, index, adjustedRect);
		}

		painter->drawText(adjustedRect, textAlignmentFlags, paintingText);

		painter->restore();
		return;
	}

	CacheKey key = std::make_pair(paintingText, std::make_pair(rect.width(), rect.height()));
	QPixmap* pixmapPointer = cached(key);

	if (!pixmapPointer)
	{
		QRect pixmapRect(0, 0, adjustedRect.width(), adjustedRect.height());

		QPixmap pixmap(pixmapRect.size());
		pixmap.fill(Qt::transparent);

		QPainter painterPixmap(&pixmap);

		if (isTextBold)
		{
			applyTextBold(&painterPixmap);
		}

		painterPixmap.setPen(textColor);

		if (isDecorationValid)
		{
			pixmapRect = paintDecorator(&painterPixmap, index, pixmapRect);
		}

		painterPixmap.drawText(pixmapRect, textAlignmentFlags, key.first);

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

QPixmap* GridViewPainterText::cached(const CacheKey& key) const
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

void GridViewPainterText::applyTextBold(QPainter* painter) const
{
	QFont font = painter->font();

	font.setBold(true);
	font.setPointSize(font.pointSize() + 1);

	painter->setFont(font);
}

QRect GridViewPainterText::paintDecorator(QPainter* painter, const QModelIndex& index, const QRect& rect) const
{
	QPixmap pixmap = qvariant_cast<QPixmap>(index.data(Qt::DecorationRole));
	QSize pixmapSize = pixmap.size();
	painter->drawPixmap(QRect(rect.topLeft(), pixmapSize), pixmap);

	return rect.adjusted(pixmapSize.width() + 5, 0, 0, 0);
}

}