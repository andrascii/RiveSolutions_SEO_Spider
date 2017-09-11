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

void TextRenderer::draw(QPainter* painter, const QStyleOptionViewItem& option, const QModelIndex& index) const
{
	const int marginTop = viewModel()->marginTop(index);
	const int marginBottom = viewModel()->marginBottom(index);
	const int marginLeft = viewModel()->marginLeft(index);
	const int marginRight = viewModel()->marginRight(index);

	QRect adjustedRect = option.rect.adjusted(marginLeft, marginTop, -marginRight, -marginBottom);
	QPixmap* pixmapPointer = cached(index);

	if (pixmapPointer && pixmapPointer->rect().size() == adjustedRect.size())
	{
		painter->drawPixmap(adjustedRect, *pixmapPointer);
		return;
	}

	QRect pixmapRect(0, 0, adjustedRect.width(), adjustedRect.height());
	QPixmap pixmap(pixmapRect.size());
	pixmap.fill(Qt::transparent);

	QPainter painterPixmap(&pixmap);

	const bool isDecorationValid = !viewModel()->pixmap(index).isNull();
	const Qt::AlignmentFlag textAlignmentFlags = viewModel()->textAlignment(index);
	const QFont& font = viewModel()->font(index).resolve(option.font);
	const QColor& textColor = viewModel()->textColor(index);
	const QString paintingText = viewModel()->displayData(index, pixmapRect);

	painterPixmap.setRenderHints(QPainter::HighQualityAntialiasing);
	painterPixmap.setFont(font);
	painterPixmap.setPen(textColor);

	if (isDecorationValid)
	{
		paintDecorator(&painterPixmap, index, viewModel()->pixmapPosition(index, pixmapRect));
	}

	painterPixmap.drawText(viewModel()->displayDataPosition(index, pixmapRect), textAlignmentFlags, paintingText);

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

void TextRenderer::addRenderer(int)
{
	ASSERT(!"Attempt to add renderer to leaf");
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

QString TextRenderer::elidedText(const QString& string, const int width, bool) const noexcept
{
	QFontMetrics fontMetrics(Application::font());
	return fontMetrics.elidedText(string, Qt::ElideRight, width);
}

void TextRenderer::paintDecorator(QPainter* painter, const QModelIndex& index, const QRect& rect) const
{
	const QPixmap& pixmap = m_viewModel->pixmap(index);
	QSize pixmapSize = pixmap.size();
	painter->drawPixmap(QRect(rect.topLeft(), pixmapSize), pixmap);
}

void TextRenderer::clearCacheIfNeeded() const noexcept
{
	if (m_cache.size() > m_cacheSize)
	{
		m_cache.clear();
	}
}

const IViewModel* TextRenderer::viewModel() const noexcept
{
	return m_viewModel;
}

}