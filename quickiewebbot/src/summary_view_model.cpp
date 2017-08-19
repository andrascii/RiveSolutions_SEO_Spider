#include "summary_view_model.h"
#include "summary_model.h"
#include "text_renderer.h"
#include "background_renderer.h"
#include "selection_background_renderer.h"
#include "quickie_web_bot_helpers.h"
#include "viewport_percent_resize_policy.h"

namespace QuickieWebBot
{

SummaryViewModel::SummaryViewModel(SummaryModel* model, QObject* parent)
	: m_model(model)
	, m_resizePolicy(std::make_unique<ViewportPercentResizePolicy>(std::vector<int>{ 85, 15 }))
{
}

int SummaryViewModel::marginTop(const QModelIndex& index) const noexcept
{
	return QuickieWebBotHelpers::pointsToPixels(2);
}

int SummaryViewModel::marginBottom(const QModelIndex& index) const noexcept
{
	return QuickieWebBotHelpers::pointsToPixels(2);
}

int SummaryViewModel::marginRight(const QModelIndex& index) const noexcept
{
	return QuickieWebBotHelpers::pointsToPixels(4);
}

int SummaryViewModel::marginLeft(const QModelIndex& index) const noexcept
{
	return QuickieWebBotHelpers::pointsToPixels(4);
}

void SummaryViewModel::resetRenderersCache() const noexcept
{
}

QList<IRenderer*> SummaryViewModel::renderers(const QModelIndex& index) const noexcept
{
	static SelectionBackgroundRenderer s_selectionBackgroundRenderer;
	static BackgroundRenderer s_backgroundRenderer;

	// disabled caching
	static TextRenderer s_textRenderer(this);

	return QList<IRenderer*>()
		<< &s_backgroundRenderer
		<< &s_selectionBackgroundRenderer
		<< &s_textRenderer;
}

}