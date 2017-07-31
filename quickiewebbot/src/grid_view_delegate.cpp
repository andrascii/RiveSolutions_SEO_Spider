#include "grid_view.h"
#include "grid_view_delegate.h"
#include "igrid_view_painter.h"
#include "grid_view_painter_background.h"
#include "grid_view_painter_text.h"
#include "igrid_model.h"

namespace QuickieWebBot
{

GridViewDelegate::GridViewDelegate(GridView* parent)
	: QStyledItemDelegate(parent)
	, m_gridView(parent)
{
	// TODO: install event filter
}

void GridViewDelegate::paint(QPainter* painter, const QStyleOptionViewItem& option, const QModelIndex& index) const
{
	foreach(IGridViewPainter* gridViewPainter, m_gridView->painters())
	{
		gridViewPainter->paint(painter, option, index);
	}
}

bool GridViewDelegate::editorEvent(QEvent* event, QAbstractItemModel*, const QStyleOptionViewItem& option, const QModelIndex& index)
{
// 	if (index.data(IGridModel::ItemTypeRole) == PageInfo::UrlItemType)
// 	{
// 		if (event->type() == QEvent::MouseButtonPress)
// 		{
// 			QDesktopServices::openUrl(index.data(Qt::DisplayRole).toString());
// 		}
// 	}
	
	return false;
}

}