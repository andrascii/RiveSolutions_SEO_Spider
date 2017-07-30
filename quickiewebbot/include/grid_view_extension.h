#pragma once

namespace QuickieWebBot
{

class GridViewExtension : public QObject
{
	Q_OBJECT
public:
	GridViewExtension(QTableView* view);

private:
	Q_SLOT void colspan(const QModelIndex& index, int span);
	
	Q_SLOT void rowsInserted(const QModelIndex &parent, int first, int last);
	Q_SLOT void rowsMoved(const QModelIndex &parent, int start, int end, const QModelIndex &destination, int row);
	Q_SLOT void rowsRemoved(const QModelIndex &parent, int first, int last);

	Q_SLOT void columnsInserted(const QModelIndex &parent, int first, int last);
	Q_SLOT void columnsMoved(const QModelIndex &parent, int start, int end, const QModelIndex &destination, int column);
	Q_SLOT void columnsRemoved(const QModelIndex &parent, int first, int last);

private:
	QTableView* m_tableView;
};

}