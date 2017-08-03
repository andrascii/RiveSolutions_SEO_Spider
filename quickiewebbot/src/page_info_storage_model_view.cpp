#include "page_info_storage_model_view.h"
#include "grid_view_resize_strategy.h"
#include "igrid_model.h"
#include "page_info.h"
#include "grid_view_painter_background.h"
#include "grid_view_painter_text.h"

namespace QuickieWebBot
{

PageInfoStorageModelView::PageInfoStorageModelView(PageInfoStorageModel* model, QObject* parent)
	: QObject(parent)
	, m_model(model)
{
}

const QFont& PageInfoStorageModelView::textFont(const QModelIndex& index) const noexcept
{
	static QFont font;

	return font;
}

const QColor& PageInfoStorageModelView::textColor(const QModelIndex& index) const noexcept
{
	static QColor color(Qt::black);
	static QColor urlColor("");

	if (index.data(IGridModel::WhatsThisRole).toInt() == PageInfo::UrlItemType)
	{
		return urlColor;
	}

	return color;
}

const QColor& PageInfoStorageModelView::backgroundColor(const QModelIndex& index) const noexcept
{
	static QColor color(Qt::transparent);

	return color;
}

const QColor& PageInfoStorageModelView::selectionBackgroundColor(const QModelIndex& index) const noexcept
{
	static QColor color(7, 160, 50, 255);

	return color;
}

const QPixmap& PageInfoStorageModelView::decoration(const QModelIndex& index) const noexcept
{
	static QPixmap decoration;
	static QPixmap urlDecoration(":/images/url-icon.png");

	if (index.data(IGridModel::WhatsThisRole).toInt() == PageInfo::UrlItemType)
	{
		return urlDecoration;
	}

	return decoration;
}

QList<IGridViewPainter*> PageInfoStorageModelView::painters(const QModelIndex& index) const noexcept
{
	static GridViewPainterBackground s_bgPainter(backgroundColor(index), backgroundColor(index));
	static GridViewPainterBackground s_selectionBgPainter(selectionBackgroundColor(index), selectionBackgroundColor(index));
	static GridViewPainterText s_textPainter;

	return QList<IGridViewPainter*>() 
		<< &s_bgPainter
		<< &s_selectionBgPainter
		<< &s_textPainter;
}

IGridViewResizeStrategy* PageInfoStorageModelView::resizeStrategy() const noexcept
{
	static GridViewResizeStrategy s_strategy;

	return &s_strategy;
}

}