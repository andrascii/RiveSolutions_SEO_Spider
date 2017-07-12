#include "grid_view_extension.h"

namespace QuickieWebBot
{

GridViewExtension::GridViewExtension(QTableView* view)
	: QObject(view)
	, m_tableView(view)
{
	QAbstractItemModel* model = view->model();

	VERIFY(QObject::connect(model, SIGNAL(colspan(const QModelIndex&, int)), this, SLOT(colspan(const QModelIndex&, int))));

	VERIFY(QObject::connect(model, SIGNAL(rowsInserted(const QModelIndex&, int, int)), this, SLOT(rowsInserted(const QModelIndex&, int, int))));
	VERIFY(QObject::connect(model, SIGNAL(rowsMoved(const QModelIndex&, int, int, const QModelIndex& , int)), this, SLOT(rowsMoved(const QModelIndex&, int, int, const QModelIndex&, int))));
	VERIFY(QObject::connect(model, SIGNAL(rowsRemoved(const QModelIndex&, int, int)), this, SLOT(rowsRemoved(const QModelIndex&, int, int))));
	
	
	VERIFY(QObject::connect(model, SIGNAL(columnsInserted(const QModelIndex&, int, int)), this, SLOT(columnsInserted(const QModelIndex&, int, int))));
	VERIFY(QObject::connect(model, SIGNAL(columnsMoved(const QModelIndex&, int, int, const QModelIndex&, int)), this, SLOT(columnsMoved(const QModelIndex&, int, int, const QModelIndex&, int))));
	VERIFY(QObject::connect(model, SIGNAL(columnsRemoved(const QModelIndex&, int, int)), this, SLOT(columnsRemoved(const QModelIndex&, int, int))));
}

void GridViewExtension::colspan(const QModelIndex& index, int span)
{
	//m_tableView->setSpan(index.row(), index.column(), 1, span);
}

void GridViewExtension::rowsInserted(const QModelIndex& parent, int first, int last)
{
	Q_UNUSED(parent);
	Q_UNUSED(first);
	Q_UNUSED(last);
	// TODO: handle this event
}

void GridViewExtension::rowsMoved(const QModelIndex& parent, int start, int end, const QModelIndex& destination, int row)
{
	Q_UNUSED(parent);
	Q_UNUSED(start);
	Q_UNUSED(end);
	Q_UNUSED(destination);
	Q_UNUSED(row);
	// TODO: handle this event
}

void GridViewExtension::rowsRemoved(const QModelIndex& parent, int first, int last)
{
	Q_UNUSED(parent);
	Q_UNUSED(first);
	Q_UNUSED(last);
	// TODO: handle this event
}

void GridViewExtension::columnsInserted(const QModelIndex& parent, int first, int last)
{
	Q_UNUSED(parent);
	Q_UNUSED(first);
	Q_UNUSED(last);
	// TODO: handle this event
}

void GridViewExtension::columnsMoved(const QModelIndex& parent, int start, int end, const QModelIndex& destination, int column)
{
	Q_UNUSED(parent);
	Q_UNUSED(start);
	Q_UNUSED(end);
	Q_UNUSED(destination);
	Q_UNUSED(column);
	// TODO: handle this event
}

void GridViewExtension::columnsRemoved(const QModelIndex& parent, int first, int last)
{
	Q_UNUSED(parent);
	Q_UNUSED(first);
	Q_UNUSED(last);
	// TODO: handle this event
}

}