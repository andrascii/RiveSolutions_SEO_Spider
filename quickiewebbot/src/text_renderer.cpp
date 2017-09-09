#include "application.h"
#include "text_renderer.h"
#include "iview_model.h"
#include "abstract_table_model.h"
#include "quickie_web_bot_helpers.h"

namespace QuickieWebBot
{

TextRenderer::TextRenderer(const IViewModel* viewModel, int cacheSize)
	: m_viewModel(viewModel)
	, m_cacheSize(cacheSize)
{
}

void TextRenderer::render(QPainter* painter, const QStyleOptionViewItem& option, const QModelIndex& index) const
{
	const int marginTop = m_viewModel->marginTop(index);
	const int marginBottom = m_viewModel->marginBottom(index);
	const int marginLeft = m_viewModel->marginLeft(index);
	const int marginRight = m_viewModel->marginRight(index);

	QRect adjustedRect = option.rect.adjusted(marginLeft, marginTop, -marginRight, -marginBottom);
	QPixmap* pixmapPointer = cached(index);

	if (pixmapPointer && pixmapPointer->rect().size() == adjustedRect.size())
	{
		painter->drawPixmap(adjustedRect, *pixmapPointer);
		return;
	}

	const bool isDecorationValid = !m_viewModel->itemPixmap(index).isNull();
	const Qt::AlignmentFlag textAlignmentFlags = m_viewModel->textAlignment(index);
	const QFont& font = m_viewModel->font(index).resolve(option.font);
	const QColor& textColor = m_viewModel->textColor(index);
	QString paintingText = qvariant_cast<QString>(index.data(Qt::DisplayRole));

	QRect pixmapRect(0, 0, adjustedRect.width(), adjustedRect.height());
	QPixmap pixmap(pixmapRect.size());
	pixmap.fill(Qt::transparent);

	QFontMetrics fontMetrics(Application::font());
	paintingText = fontMetrics.elidedText(paintingText, Qt::ElideRight, adjustedRect.width());

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

void TextRenderer::invalidateCacheIndex(const QModelIndex& index) const
{
	m_cache.erase(index);
}

void TextRenderer::invalidateCache() const
{
	m_cache.clear();
}

void TextRenderer::setCacheSize(int cacheSize)
{
	m_cacheSize = cacheSize;

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
	const QPixmap& pixmap = m_viewModel->itemPixmap(index);

	QSize pixmapSize = pixmap.size();

	painter->drawPixmap(QRect(rect.topLeft(), pixmapSize), pixmap);

	return rect.adjusted(pixmapSize.width() + 5, 0, 0, 0);
}

void TextRenderer::clearCacheIfNeeded() const noexcept
{
	if (m_cache.size() > m_cacheSize)
	{
		m_cache.clear();
	}
}

}