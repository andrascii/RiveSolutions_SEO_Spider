#include "table_model.h"

namespace QuickieWebBot
{

TableModel::TableModel()
{

}

int TableModel::rowCount(QModelIndex const& parent) const
{
	return 0;
}

int TableModel::columnCount(QModelIndex const& parent) const
{
	return 0;
}

QVariant TableModel::data(QModelIndex const& index, int role) const
{
	return 0;
}

QVariant TableModel::headerData(int section, Qt::Orientation orientation, int role) const
{
	return 0;
}

bool TableModel::insertRows(int row, int count, QModelIndex const& parent)
{
	return 0;
}

bool TableModel::removeRows(int row, int count, QModelIndex const& parent)
{
	return 0;
}

bool TableModel::insertColumns(int column, int count, QModelIndex const& parent)
{
	return 0;
}

bool TableModel::removeColumns(int column, int count, QModelIndex const& parent)
{
	return 0;
}

}