#include "url_renderer.h"
#include "quickie_web_bot_helpers.h"

namespace QuickieWebBot
{

UrlRenderer::UrlRenderer(const IViewModel* viewModel, int maxCacheSize)
	: TextRenderer(viewModel, maxCacheSize)
{
}

QRect UrlRenderer::paintDecorator(QPainter* painter, const QModelIndex& index, const QRect& rect) const
{
	QPixmap pixmap = index.data(Qt::DecorationRole).value<QPixmap>();

	const int offsetByX = rect.width() - QuickieWebBotHelpers::pointsToPixels(30);
	const int offsetByY = rect.y() + QuickieWebBotHelpers::pointsToPixels(2);

	painter->drawPixmap(offsetByX, offsetByY, QuickieWebBotHelpers::pointsToPixels(15), QuickieWebBotHelpers::pointsToPixels(15), pixmap);

	return rect.adjusted(pixmap.size().width() + 5, 0, 0, 0);
}

}