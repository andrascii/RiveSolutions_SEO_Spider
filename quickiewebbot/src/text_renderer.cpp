#include "text_renderer.h"
#include "igrid_view_model.h"
#include "quickie_web_bot_helpers.h"

namespace QuickieWebBot
{

TextRenderer::TextRenderer(const IGridViewModel* viewModel, int maxCacheSize)
	: m_viewModel(viewModel)
	, m_maxCacheSize(maxCacheSize)
	, m_marginLeft(QuickieWebBotHelpers::pointsToPixels(4))
	, m_marginTop(QuickieWebBotHelpers::pointsToPixels(2))
	, m_marginRight(QuickieWebBotHelpers::pointsToPixels(4))
	, m_marginBottom(QuickieWebBotHelpers::pointsToPixels(2))
{
}

void TextRenderer::render(QPainter* painter, const QStyleOptionViewItem& option, const QModelIndex& index) const
{
	const bool isDecorationValid = m_viewModel->decoration(index).isNull();
	const int textAlignmentFlags = m_viewModel->textAlignmentFlags(index);
	const QFont& font = m_viewModel->textFont(index);
	const QString paintingText = index.data(Qt::DisplayRole).toString();
	const QColor& textColor = m_viewModel->textColor(index);

	QRect adjustedRect = option.rect.adjusted(m_marginLeft, m_marginTop, -m_marginRight, -m_marginBottom);

	QPixmap* pixmapPointer = cached(index);

	if (pixmapPointer == nullptr || pixmapPointer->rect().size() != adjustedRect.size())
	{
		QRect pixmapRect(0, 0, adjustedRect.width(), adjustedRect.height());

		QPixmap pixmap(pixmapRect.size());
		pixmap.fill(Qt::transparent);

		QPainter painterPixmap(&pixmap);
		painterPixmap.setFont(font);
		painterPixmap.setPen(textColor);

		if (isDecorationValid)
		{
			pixmapRect = paintDecorator(&painterPixmap, index, pixmapRect);
		}

		painterPixmap.drawText(pixmapRect, textAlignmentFlags, paintingText);
		m_cache[index] = pixmap;
		pixmapPointer = &m_cache[index];
	}

	painter->drawPixmap(adjustedRect, *pixmapPointer);
	clearCacheIfNeeded();
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
	const QPixmap& pixmap = m_viewModel->decoration(index);

	QSize pixmapSize = pixmap.size();

	painter->drawPixmap(QRect(rect.topLeft(), pixmapSize), pixmap);

	return rect.adjusted(pixmapSize.width() + 5, 0, 0, 0);
}

void TextRenderer::clearCacheIfNeeded() const noexcept
{
	if (m_cache.size() > m_maxCacheSize * 2)
	{
		m_cache.clear();
	}
}

}