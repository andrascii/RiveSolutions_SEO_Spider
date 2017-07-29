#include "application.h"
#include "grid_view_model.h"
#include "quickie_web_bot_helpers.h"

namespace QuickieWebBot
{

GridViewModel::GridViewModel(QObject* parent)
	: QAbstractTableModel(parent)
{
}

int GridViewModel::rowCount(QModelIndex const&) const
{
	return m_accessor->itemCount();
}

int GridViewModel::columnCount(QModelIndex const&) const
{
	return m_accessor->supportedColumns().size();
}

QVariant GridViewModel::data(QModelIndex const& index, int role) const
{
	if (role == Qt::DisplayRole)
	{
		return m_accessor->item(index);
	}

	if (role == Qt::DecorationRole)
	{
// 		QPixmap* pixmap = m_accessor->pixmap(index);
// 
// 		if (pixmap != nullptr)
// 		{
// 			return *pixmap;
// 		}
	}

	if (role == Qt::BackgroundColorRole)
	{
		//return m_accessor->itemBackgroundColor(index);
	}

	if (role == Qt::TextColorRole)
	{
		//return m_accessor->itemTextColor(index);
	}

	if (role == Qt::UserRole)
	{
		//return m_accessor->flags(index);
	}

	return QVariant();
}

QVariant GridViewModel::headerData(int section, Qt::Orientation orientation, int role) const
{
	if (orientation == Qt::Vertical)
	{
		return section;
	}

	if (role == Qt::DisplayRole)
	{
		return PageInfo::itemTitle(m_accessor->supportedColumns()[section]);
	}

	return QVariant();
}

void GridViewModel::setModelDataAccessor(std::unique_ptr<IModelDataAccessor> accessor)
{
	if (m_accessor)
	{
		VERIFY(disconnect(m_accessor->qobject(), SIGNAL(rowAdded(int)), 
			this, SLOT(onRowAdded(int))));

		VERIFY(disconnect(m_accessor->qobject(), SIGNAL(itemChanged(int, int)),
			this, SLOT(onItemChanged(int, int))));
	}

	std::unique_ptr<IModelDataAccessor> oldAccessor = std::move(m_accessor);
	m_accessor = std::move(accessor);

	VERIFY(connect(m_accessor->qobject(), SIGNAL(rowAdded(int)), this, SLOT(onRowAdded(int))));
	VERIFY(connect(m_accessor->qobject(), SIGNAL(itemChanged(int, int)), this, SLOT(onItemChanged(int, int))));

	emit modelDataAccessorChanged(m_accessor.get(), oldAccessor.get());
	
	beginResetModel();
	endResetModel();
}

const IModelDataAccessor* GridViewModel::modelDataAcessor() const
{
	return m_accessor.get();
}

IModelDataAccessor* GridViewModel::modelDataAcessor()
{
	return m_accessor.get();
}

const IGridViewResizeStrategy* GridViewModel::resizeStrategy() const
{
	return m_accessor ? m_accessor->resizeStrategy() : nullptr;
}

IGridViewResizeStrategy* GridViewModel::resizeStrategy()
{
	return m_accessor ? m_accessor->resizeStrategy() : nullptr;
}

void GridViewModel::onRowAdded(int row)
{
	beginInsertRows(QModelIndex(), row, row);
	endInsertRows();
}

void GridViewModel::onItemChanged(int row, int column)
{
	QModelIndex idx = index(row, column);
	emit dataChanged(idx, idx);
}

}