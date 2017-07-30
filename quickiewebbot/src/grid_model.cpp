#include "application.h"
#include "grid_model.h"
#include "quickie_web_bot_helpers.h"

namespace QuickieWebBot
{

GridModel::GridModel(QObject* parent)
	: QAbstractTableModel(parent)
{
}

Qt::ItemFlags GridModel::flags(const QModelIndex &index) const
{
	return Qt::NoItemFlags;
}

int GridModel::rowCount(QModelIndex const&) const
{
	return m_accessor->itemCount();
}

int GridModel::columnCount(QModelIndex const&) const
{
	return m_accessor->columnsCount();
}

QVariant GridModel::data(QModelIndex const& index, int role) const
{
	if (role == Qt::DisplayRole)
	{
		return m_accessor->item(index);
	}

	if (role == CustomRoles::ItemTypeRole)
	{
		return m_accessor->itemType(index);
	}

	return QVariant();
}

QVariant GridModel::headerData(int section, Qt::Orientation orientation, int role) const
{
	if (orientation == Qt::Vertical)
	{
		return section;
	}

	if (role == Qt::DisplayRole)
	{
		return m_accessor->columnDescription(section);
	}

	return QVariant();
}

void GridModel::setModelDataAccessor(std::unique_ptr<IGridDataAccessor> accessor)
{
	if (m_accessor)
	{
		VERIFY(disconnect(m_accessor->qobject(), SIGNAL(rowAdded(int)), 
			this, SLOT(onRowAdded(int))));

		VERIFY(disconnect(m_accessor->qobject(), SIGNAL(itemChanged(int, int)),
			this, SLOT(onItemChanged(int, int))));
	}

	std::unique_ptr<IGridDataAccessor> oldAccessor = std::move(m_accessor);
	m_accessor = std::move(accessor);

	VERIFY(connect(m_accessor->qobject(), SIGNAL(rowAdded(int)), this, SLOT(onRowAdded(int))));
	VERIFY(connect(m_accessor->qobject(), SIGNAL(itemChanged(int, int)), this, SLOT(onItemChanged(int, int))));

	emit modelDataAccessorChanged(m_accessor.get(), oldAccessor.get());
	
	beginResetModel();
	endResetModel();
}

const IGridDataAccessor* GridModel::modelDataAcessor() const
{
	return m_accessor.get();
}

IGridDataAccessor* GridModel::modelDataAcessor()
{
	return m_accessor.get();
}

const IGridViewResizeStrategy* GridModel::resizeStrategy() const
{
	return m_accessor ? m_accessor->resizeStrategy() : nullptr;
}

IGridViewResizeStrategy* GridModel::resizeStrategy()
{
	return m_accessor ? m_accessor->resizeStrategy() : nullptr;
}

void GridModel::onRowAdded(int row)
{
	beginInsertRows(QModelIndex(), row, row);
	endInsertRows();
}

void GridModel::onItemChanged(int row, int column)
{
	QModelIndex idx = index(row, column);

	Q_EMIT dataChanged(idx, idx);
}

}