#include "url_renderer.h"
#include "quickie_web_bot_helpers.h"
#include "iview_model.h"

namespace QuickieWebBot
{

UrlRenderer::UrlRenderer(const IViewModel* viewModel, int maxCacheSize)
	: TextRenderer(viewModel, maxCacheSize)
	, m_viewModel(viewModel)
{
}


QString UrlRenderer::elidedText(const QString& string, const int width, bool isDecorationValid) const noexcept
{
	const int correctedWidth = isDecorationValid ? width - QuickieWebBotHelpers::pointsToPixels(20) : width;

	return TextRenderer::elidedText(string, correctedWidth, isDecorationValid);
}

QRect UrlRenderer::paintDecorator(QPainter* painter, const QModelIndex& index, const QRect& rect) const
{
	const QPixmap& pixmap = m_viewModel->itemPixmap(index);

	const int offsetByX = rect.width() - QuickieWebBotHelpers::pointsToPixels(20);

	painter->drawPixmap(offsetByX, 0, pixmap.size().width(), pixmap.size().height(), pixmap);

	return rect;
}

}