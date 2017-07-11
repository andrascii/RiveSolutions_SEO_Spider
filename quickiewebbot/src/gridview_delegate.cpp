#include "gridview_delegate.h"
#include "imodel_data_accessor.h"
#include "gridview_painter.h"
#include "gridview.h"

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
	IModelDataAccessor* accessor = m_gridView->dataAccessor();
	assert(accessor != nullptr);

	const std::vector<GridViewPainter*> painters = accessor->painters(index);
	for (auto it = std::begin(painters); it < std::end(painters); it++)
	{
		(*it)->paint(painter, option.rect, index);
	}
}

bool GridViewDelegate::editorEvent(QEvent* event, QAbstractItemModel*, const QStyleOptionViewItem& option, const QModelIndex& index)
{
	if (index.data(Qt::UserRole).toInt() & IModelDataAccessor::ItemFlagUrl)
	{
		if (event->type() == QEvent::MouseButtonRelease)
		{
			// TODO: open url
		}

	}
	
	return false;
}

}