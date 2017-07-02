#include "table_model.h"

namespace QuickieWebBot
{

TableModel::TableModel(QObject* parent)
	: QAbstractTableModel(parent)
{
}

TableModel::TableModel(QVector<WebsiteAnalyseElement> const * const dataStorage, QObject* parent)
	: QAbstractTableModel(parent)
	, ModelData(dataStorage)
{
}

int TableModel::rowCount(QModelIndex const& parent) const
{
	return dataStorage().size();
}

int TableModel::columnCount(QModelIndex const& parent) const
{
	return headerItems().size();
}

QVariant TableModel::data(QModelIndex const& index, int role) const
{
	if (role == Qt::DisplayRole)
	{
		return dataStorage()[index.row()].title;
	}

	if (role == Qt::BackgroundColorRole)
	{
		return QColor(Qt::white);
	}

	return QVariant();
}

QVariant TableModel::headerData(int section, Qt::Orientation orientation, int role) const
{
	if (orientation == Qt::Vertical)
	{
		return QVariant();
	}

	if (role == Qt::DisplayRole)
	{
		return headerItems()[section];
	}

	return QVariant();
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