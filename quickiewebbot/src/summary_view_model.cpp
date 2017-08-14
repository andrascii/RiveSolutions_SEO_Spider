#include "summary_view_model.h"
#include "summary_model.h"
#include "text_renderer.h"
#include "background_renderer.h"
#include "selection_background_renderer.h"
#include "quickie_web_bot_helpers.h"
#include "full_size_resize_policy.h"

namespace QuickieWebBot
{

SummaryViewModel::SummaryViewModel(SummaryModel* model, QObject* parent)
	: m_model(model)
	, m_resizePolicy(std::make_unique<FullSizeResizePolicy>(std::vector<int>{ 85, 15 }))
{
}

void SummaryViewModel::resetRenderersCache() const noexcept
{
}

QList<IRenderer*> SummaryViewModel::renderers(const QModelIndex& index) const noexcept
{
	static SelectionBackgroundRenderer s_selectionBackgroundRenderer;
	static BackgroundRenderer s_backgroundRenderer;

	// disabled caching
	static TextRenderer s_textRenderer;

	return QList<IRenderer*>()
		<< &s_backgroundRenderer
		<< &s_selectionBackgroundRenderer
		<< &s_textRenderer;
}

IResizePolicy* SummaryViewModel::resizePolicy() const noexcept
{
	return m_resizePolicy.get();
}

}