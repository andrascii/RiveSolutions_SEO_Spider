#include "page_info.h"
#include "igrid_model.h"
#include "text_renderer.h"
#include "grid_view_resize_strategy.h"
#include "page_info_storage_view_model.h"
#include "background_renderer.h"
#include "selection_background_renderer.h"

namespace QuickieWebBot
{

PageInfoStorageViewModel::PageInfoStorageViewModel(PageInfoStorageModel* model, QObject* parent)
	: QObject(parent)
	, m_model(model)
{
}

const QFont& PageInfoStorageViewModel::textFont(const QModelIndex& index) const noexcept
{
	static QFont font;

	return font;
}

const QColor& PageInfoStorageViewModel::textColor(const QModelIndex& index) const noexcept
{
	static QColor color(Qt::black);
	static QColor urlColor("#4753C5");

	if (index.data(IGridModel::WhatsThisRole).toInt() == PageInfo::UrlItemType)
	{
		return urlColor;
	}

	return color;
}

const QColor& PageInfoStorageViewModel::backgroundColor(const QModelIndex& index) const noexcept
{
	static QColor color(Qt::transparent);

	return color;
}

const QColor& PageInfoStorageViewModel::selectionBackgroundColor(const QModelIndex& index) const noexcept
{
	static QColor color(7, 160, 50, 255);

	return color;
}

const QPixmap& PageInfoStorageViewModel::decoration(const QModelIndex& index) const noexcept
{
	static QPixmap decoration;
	static QPixmap urlDecoration(":/images/url-icon.png");

	if (index.data(IGridModel::WhatsThisRole).toInt() == PageInfo::UrlItemType)
	{
		return urlDecoration;
	}

	return decoration;
}

int PageInfoStorageViewModel::textAlignmentFlags(const QModelIndex& index) const noexcept
{
	return Qt::AlignVCenter | Qt::AlignLeft;
}

QList<IRenderer*> PageInfoStorageViewModel::renderers(const QModelIndex& index) const noexcept
{
	static BackgroundRenderer s_bgPainter(backgroundColor(index), backgroundColor(index));
	static SelectionBackgroundRenderer s_selectionBgPainter(selectionBackgroundColor(index), selectionBackgroundColor(index));
	static TextRenderer s_textPainter(this, 1000);

	return QList<IRenderer*>()
		<< &s_selectionBgPainter
		<< &s_bgPainter
		<< &s_textPainter;
}

IGridViewResizePolicy* PageInfoStorageViewModel::resizeStrategy() const noexcept
{
	static GridViewResizeStrategy s_strategy;

	return &s_strategy;
}

}