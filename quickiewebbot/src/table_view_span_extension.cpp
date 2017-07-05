#include "table_view_span_extension.h"

namespace QuickieWebBot
{

TableViewSpanExtension::TableViewSpanExtension(QTableView* view)
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

void TableViewSpanExtension::colspan(const QModelIndex& index, int span)
{
	m_tableView->setSpan(index.row(), index.column(), 1, span);
}

void TableViewSpanExtension::rowsInserted(const QModelIndex & parent, int first, int last)
{
	// TODO: handle this event
}

void TableViewSpanExtension::rowsMoved(const QModelIndex & parent, int start, int end, const QModelIndex & destination, int row)
{
	// TODO: handle this event
}

void TableViewSpanExtension::rowsRemoved(const QModelIndex & parent, int first, int last)
{
	// TODO: handle this event
}

void TableViewSpanExtension::columnsInserted(const QModelIndex & parent, int first, int last)
{
	// TODO: handle this event
}

void TableViewSpanExtension::columnsMoved(const QModelIndex & parent, int start, int end, const QModelIndex & destination, int column)
{
	// TODO: handle this event
}

void TableViewSpanExtension::columnsRemoved(const QModelIndex & parent, int first, int last)
{
	// TODO: handle this event
}

}