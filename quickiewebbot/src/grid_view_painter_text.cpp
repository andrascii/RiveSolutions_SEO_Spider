#include "grid_view_painter_text.h"
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

void GridViewPainterText::paint(QPainter* painter, const QStyleOptionViewItem& option, const QModelIndex& index) const
{
	const bool isTextBold = false;// index.data(Qt::FontRole).value<QFont>().bold();
	const bool isDecorationValid = false;// index.data(Qt::DecorationRole).isValid();

	const int textAlignmentFlags = Qt::AlignTop;// index.data(Qt::TextAlignmentRole).toInt() | Qt::AlignTop;
	const QString paintingText = index.data(Qt::DisplayRole).toString();
	const QColor textColor = QColor(Qt::black);// qvariant_cast<QColor>(index.data(Qt::TextColorRole));

	QRect adjustedRect = option.rect.adjusted(m_marginLeft, m_marginTop, -m_marginRight, -m_marginBottom);

	painter->save();
	painter->setPen(textColor);

	if (isTextBold)
	{
		applyTextBold(painter);
	}

	if (isDecorationValid)
	{
		adjustedRect = paintDecorator(painter, index, adjustedRect);
	}

	painter->setClipRect(adjustedRect);
	painter->drawText(adjustedRect, textAlignmentFlags, paintingText);
	painter->restore();
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