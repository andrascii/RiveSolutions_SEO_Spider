#include "summary_view_model.h"
#include "summary_model.h"
#include "text_renderer.h"
#include "background_renderer.h"
#include "selection_background_renderer.h"
#include "quickie_web_bot_helpers.h"
#include "grid_view_full_size_resize_strategy.h"

namespace QuickieWebBot
{

SummaryViewModel::SummaryViewModel(SummaryModel* model, QObject* parent)
	: m_model(model)
	, m_resizeStrategy(std::make_unique<GridViewFullSizeResizeStrategy>(std::vector<int>{ 60, 40 }))
{
}

QList<IRenderer*> SummaryViewModel::renderers(const QModelIndex& index) const noexcept
{
	static SelectionBackgroundRenderer s_selectionBgPainter;
	static BackgroundRenderer s_bgPainter;
	static TextRenderer s_textPainter(this, std::pow(m_model->columnCount(index), 2));

	return QList<IRenderer*>()
		<< &s_bgPainter
		<< &s_selectionBgPainter
		<< &s_textPainter;
}

IGridViewResizePolicy* SummaryViewModel::resizeStrategy() const noexcept
{
	return m_resizeStrategy.get();
}

}