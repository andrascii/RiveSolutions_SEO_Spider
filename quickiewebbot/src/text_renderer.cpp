#include "text_renderer.h"
#include "igrid_view_model.h"
#include "igrid_model.h"
#include "quickie_web_bot_helpers.h"

namespace QuickieWebBot
{

TextRenderer::TextRenderer(int maxCacheSize)
	: m_maxCacheSize(maxCacheSize)
{
}

void TextRenderer::render(QPainter* painter, const QStyleOptionViewItem& option, const QModelIndex& index) const
{
	int marginTop = index.data(IGridModel::MarginTop).toInt();
	int marginBottom = index.data(IGridModel::MarginBottom).toInt();
	int marginLeft = index.data(IGridModel::MarginLeft).toInt();
	int marginRight = index.data(IGridModel::MarginRight).toInt();

	QRect adjustedRect = option.rect.adjusted(marginLeft, marginTop, -marginRight, -marginBottom);
	QPixmap* pixmapPointer = cached(index);

	if (pixmapPointer && pixmapPointer->rect().size() == adjustedRect.size())
	{
		painter->drawPixmap(adjustedRect, *pixmapPointer);
		return;
	}

	const bool isDecorationValid = index.data(Qt::DecorationRole).isValid();
	const int textAlignmentFlags = index.data(Qt::TextAlignmentRole).toInt();
	const QFont& font = index.data(Qt::FontRole).value<QFont>();
	const QString paintingText = index.data(Qt::DisplayRole).toString();
	const QColor& textColor = index.data(Qt::TextColorRole).value<QColor>();

	QRect pixmapRect(0, 0, adjustedRect.width(), adjustedRect.height());
	QPixmap pixmap(pixmapRect.size());
	pixmap.fill(Qt::transparent);

	QPainter painterPixmap(&pixmap);

	painterPixmap.setRenderHints(QPainter::HighQualityAntialiasing);
	painterPixmap.setFont(font);
	painterPixmap.setPen(textColor);

	if (isDecorationValid)
	{
		pixmapRect = paintDecorator(&painterPixmap, index, pixmapRect);
	}

	painterPixmap.drawText(pixmapRect, textAlignmentFlags, paintingText);
	m_cache[index] = pixmap;
	pixmapPointer = &m_cache[index];

	painter->drawPixmap(adjustedRect, *pixmapPointer);

	clearCacheIfNeeded();
}

void TextRenderer::resetCache() const
{
	m_cache.clear();
}

QPixmap* TextRenderer::cached(const QModelIndex& index) const
{
	auto iter = m_cache.find(index);

	if (iter != std::end(m_cache))
	{
		return &iter->second;
	}

	return nullptr;
}

QRect TextRenderer::paintDecorator(QPainter* painter, const QModelIndex& index, const QRect& rect) const
{
	QPixmap pixmap = index.data(Qt::DecorationRole).value<QPixmap>();

	QSize pixmapSize = pixmap.size();

	painter->drawPixmap(QRect(rect.topLeft(), pixmapSize), pixmap);

	return rect.adjusted(pixmapSize.width() + 5, 0, 0, 0);
}

void TextRenderer::clearCacheIfNeeded() const noexcept
{
	if (m_cache.size() > m_maxCacheSize)
	{
		m_cache.clear();
	}
}

}