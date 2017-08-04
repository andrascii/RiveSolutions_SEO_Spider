#include "page_info.h"
#include "igrid_model.h"
#include "grid_view_resize_policy.h"
#include "text_renderer.h"
#include "background_renderer.h"
#include "selection_background_renderer.h"
#include "page_info_storage_model.h"
#include "page_info_storage_view_model.h"
#include "quickie_web_bot_helpers.h"


namespace QuickieWebBot
{

PageInfoStorageViewModel::PageInfoStorageViewModel(PageInfoStorageModel* model, QObject* parent)
	: QObject(parent)
	, m_model(model)
	, m_resizePolicy(std::make_unique<GridViewResizePolicy>())
{
}

QList<IRenderer*> PageInfoStorageViewModel::renderers(const QModelIndex& index) const noexcept
{
	static TextRenderer s_textPainter(this, std::pow(m_model->columnCount(index), 2));
	static SelectionBackgroundRenderer s_selectionBgPainter;
	static BackgroundRenderer s_bgPainter;

	return QList<IRenderer*>()
		<< &s_bgPainter
		<< &s_selectionBgPainter
		<< &s_textPainter;
}

IGridViewResizePolicy* PageInfoStorageViewModel::resizePolicy() const noexcept
{
	return m_resizePolicy.get();
}

}