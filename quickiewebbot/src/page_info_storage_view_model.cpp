#include "page_info.h"
#include "igrid_model.h"
#include "grid_view_resize_strategy.h"

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
{
}

int PageInfoStorageViewModel::marginTop(const QModelIndex& index) const noexcept
{
	index;

	return QuickieWebBotHelpers::pointsToPixels(2);
}

int PageInfoStorageViewModel::marginBottom(const QModelIndex& index) const noexcept
{
	index;

	return QuickieWebBotHelpers::pointsToPixels(2);
}

int PageInfoStorageViewModel::marginLeft(const QModelIndex& index) const noexcept
{
	index;

	return QuickieWebBotHelpers::pointsToPixels(4);
}

int PageInfoStorageViewModel::marginRight(const QModelIndex& index) const noexcept
{
	index;

	return QuickieWebBotHelpers::pointsToPixels(4);
}

QList<IRenderer*> PageInfoStorageViewModel::renderers(const QModelIndex& index) const noexcept
{
	static SelectionBackgroundRenderer s_selectionBgPainter;
	static BackgroundRenderer s_bgPainter;
	static TextRenderer s_textPainter(this, std::pow(m_model->columnCount(index), 2));

	return QList<IRenderer*>()
		<< &s_bgPainter
		<< &s_selectionBgPainter
		<< &s_textPainter;
}

IGridViewResizePolicy* PageInfoStorageViewModel::resizeStrategy() const noexcept
{
	static GridViewResizeStrategy s_strategy;

	return &s_strategy;
}

}