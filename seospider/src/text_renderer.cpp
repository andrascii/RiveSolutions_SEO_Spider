#include "stdafx.h"
#include "application.h"
#include "text_renderer.h"
#include "iview_model.h"
#include "model_helpers.h"

namespace SeoSpider
{

TextRenderer::TextRenderer(const IViewModel* viewModel, int cacheSize)
	: m_viewModel(viewModel)
	, m_cache(cacheSize)
{
	ASSERT(cacheSize > 0);
}

void TextRenderer::draw(QPainter* painter, const QStyleOptionViewItem& option, const QModelIndex& index) const
{
	const QModelIndex underlyingIndex = getUnderlyingIndex(index);
    const double devicePixelRatio = viewModel()->devicePixelRatio();

	QPixmap* pixmapPointer = m_cache.object(underlyingIndex);

	if (pixmapPointer && pixmapPointer->rect().size() == option.rect.size())
	{
		painter->drawPixmap(option.rect, *pixmapPointer);
		return;
	}

	QRect pixmapRect(0, 0, option.rect.width() * devicePixelRatio, option.rect.height() * devicePixelRatio);
	pixmapPointer = new QPixmap(pixmapRect.size());
	pixmapPointer->setDevicePixelRatio(devicePixelRatio);
	pixmapPointer->fill(Qt::transparent);

	QPainter painterPixmap(pixmapPointer);

	const bool isDecorationValid = !viewModel()->pixmap(underlyingIndex).isNull();
	const int textAlignmentFlags = viewModel()->textAlignment(underlyingIndex);
	const QFont& font = viewModel()->font(underlyingIndex);
	const QColor& textColor = viewModel()->textColor(underlyingIndex);
	const QString paintingText = viewModel()->displayData(underlyingIndex, pixmapRect);

    painterPixmap.setRenderHints(QPainter::Antialiasing | QPainter::SmoothPixmapTransform | QPainter::TextAntialiasing);
    painterPixmap.setFont(font);
    painterPixmap.setPen(textColor);

	if (isDecorationValid)
	{
		paintDecorator(&painterPixmap, underlyingIndex, viewModel()->pixmapPosition(underlyingIndex, pixmapRect));
	}

    QRect displayDataPosition = viewModel()->displayDataPosition(underlyingIndex, pixmapRect);

    if (devicePixelRatio > 1.0)
    {
        displayDataPosition.adjust(0, 0, -displayDataPosition.width() / devicePixelRatio, 0);
    }

    painterPixmap.drawText(displayDataPosition, textAlignmentFlags, paintingText);

	ASSERT(m_cache.insert(underlyingIndex, pixmapPointer));
	painter->drawPixmap(option.rect, *pixmapPointer);
}

void TextRenderer::invalidateCacheIndex(const QModelIndex& index) const
{
	m_cache.remove(getUnderlyingIndex(index));
}

void TextRenderer::invalidateCache() const
{
	m_cache.clear();
}

void TextRenderer::setCacheSize(int cacheSize)
{
	ASSERT(cacheSize > 0);
	m_cache.setMaxCost(cacheSize);
}

void TextRenderer::addRenderer(int)
{
	ASSERT(!"Attempt to add renderer to leaf");
}

QString TextRenderer::elidedText(const QString& string, const int width, bool) const noexcept
{
	QFontMetrics fontMetrics(Application::font());
	return fontMetrics.elidedText(string, Qt::ElideRight, width);
}

void TextRenderer::paintDecorator(QPainter* painter, const QModelIndex& index, const QRect& rect) const
{
	const QPixmap& pixmap = m_viewModel->pixmap(index);
	const QSize pixmapSize = pixmap.size();
	painter->drawPixmap(QRect(rect.topLeft(), pixmapSize), pixmap);
}

const IViewModel* TextRenderer::viewModel() const noexcept
{
	return m_viewModel;
}

}
