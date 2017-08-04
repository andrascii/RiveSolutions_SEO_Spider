#include "text_renderer.h"
#include "igrid_view_model.h"
#include "quickie_web_bot_helpers.h"

namespace QuickieWebBot
{

TextRenderer::TextRenderer(const IGridViewModel* viewModel, int maxCacheSize)
	: m_viewModel(viewModel)
	, m_maxCacheSize(maxCacheSize)
{
}

void TextRenderer::render(QPainter* painter, const QStyleOptionViewItem& option, const QModelIndex& index) const
{
	QRect adjustedRect = option.rect.adjusted(
		m_viewModel->marginLeft(index),
		m_viewModel->marginTop(index),
		-m_viewModel->marginRight(index),
		-m_viewModel->marginBottom(index)
	);

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

	//
	// TODO: Refactor. Incorrect call of method with side-effect from const method
	//
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