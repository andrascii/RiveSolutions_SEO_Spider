#include "grid_view_delegate.h"
#include "imodel_data_accessor.h"
#include "grid_view_painter.h"
#include "grid_view.h"
#include "grid_view_painter_background.h"
#include "grid_view_painter_text.h"

namespace QuickieWebBot
{

GridViewDelegate::GridViewDelegate(GridView* parent)
	: QStyledItemDelegate(parent)
	, m_gridView(parent)
{
	// TODO: install eventfilter
}

void GridViewDelegate::paint(QPainter* painter, const QStyleOptionViewItem& option, const QModelIndex& index) const
{
	IModelDataAccessor* accessor = m_gridView->modelDataAccessor();
	assert(accessor != nullptr);

	if (option.state & QStyle::State_Selected)
	{
		static GridViewPainterBackground s_selectionPainter(QColor(0, 50, 100, 50), QColor(0, 50, 100, 50));
		s_selectionPainter.paint(painter, option.rect, index);
		accessor->textPainter(index)->paint(painter, option.rect, index);

		return;
	}

	if (m_gridView->hoveredIndex().row() == index.row())
	{
		static GridViewPainterBackground s_hoveredRowsPainter(QColor(224, 224, 224, 255), QColor(224, 224, 224, 255));
		s_hoveredRowsPainter.paint(painter, option.rect, index);
	}

	accessor->textPainter(index)->paint(painter, option.rect, index);
}

bool GridViewDelegate::editorEvent(QEvent* event, QAbstractItemModel*, const QStyleOptionViewItem& option, const QModelIndex& index)
{
	if (index.data(Qt::UserRole).toInt() & IModelDataAccessor::ItemFlagUrl)
	{
		if (event->type() == QEvent::MouseButtonPress)
		{
			QDesktopServices::openUrl(index.data(Qt::DisplayRole).toString());
		}
	}
	
	return false;
}

}