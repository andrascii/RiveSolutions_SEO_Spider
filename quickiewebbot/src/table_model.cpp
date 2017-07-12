#include "table_model.h"

namespace QuickieWebBot
{

TableModel::TableModel(QObject* parent)
	: QAbstractTableModel(parent)
{
}

int TableModel::rowCount(QModelIndex const& parent) const
{
	return m_accessor->rowCount();
}

int TableModel::columnCount(QModelIndex const& parent) const
{
	return m_accessor->columnCount();
}

QVariant TableModel::data(QModelIndex const& index, int role) const
{
	if (role == Qt::DisplayRole)
	{
		return m_accessor->itemValue(index);
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
		return m_accessor->columnText(section);
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

void TableModel::setDataAccessor(std::unique_ptr<IModelDataAccessorItem> accessor)
{
	if (m_accessor)
	{
		VERIFY(QObject::disconnect(m_accessor->qobject(), SIGNAL(rowAdded(int)), this, SLOT(onRowAdded(int))));
	}

	m_accessor = std::move(accessor);

	VERIFY(QObject::connect(m_accessor->qobject(), SIGNAL(rowAdded(int)), this, SLOT(onRowAdded(int))));
}

void TableModel::onRowAdded(int row)
{
	INFOLOG("rows", "new row added");
	beginInsertRows(QModelIndex(), row, row);
	endInsertRows();
}

}