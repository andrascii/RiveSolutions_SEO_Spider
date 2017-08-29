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

QRect UrlRenderer::paintDecorator(QPainter* painter, const QModelIndex& index, const QRect& rect) const
{
	const QPixmap& pixmap = m_viewModel->itemPixmap(index);

	const int offsetByX = rect.width() - QuickieWebBotHelpers::pointsToPixels(30);
	const int offsetByY = rect.y() + QuickieWebBotHelpers::pointsToPixels(2);

	painter->drawPixmap(offsetByX, offsetByY, QuickieWebBotHelpers::pointsToPixels(15), QuickieWebBotHelpers::pointsToPixels(15), pixmap);

	return rect;
}

}