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
	return m_accessor->rowCount();
}

int GridViewModel::columnCount(QModelIndex const&) const
{
	return m_accessor->columnCount();
}

QVariant GridViewModel::data(QModelIndex const& index, int role) const
{
	if (role == Qt::DisplayRole)
	{
		emit colspan(index, m_accessor->itemColSpan(index));
		return m_accessor->itemValue(index);
	}

	if (role == Qt::DecorationRole)
	{
		QPixmap* pixmap = m_accessor->pixmap(index);
		if (pixmap != nullptr)
		{
			return *pixmap;
		}
	}

	if (role == Qt::BackgroundColorRole)
	{
		return m_accessor->itemBackgroundColor(index);
	}

	if (role == Qt::TextColorRole)
	{
		return m_accessor->itemTextColor(index);
	}

	if (role == Qt::UserRole)
	{
		return m_accessor->flags(index);
	}

	return QVariant();
}

QVariant GridViewModel::headerData(int section, Qt::Orientation orientation, int role) const
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

bool GridViewModel::insertRows(int row, int count, QModelIndex const& parent)
{
	Q_UNUSED(parent);
	Q_UNUSED(count);
	Q_UNUSED(row);
	return 0;
}

bool GridViewModel::removeRows(int row, int count, QModelIndex const& parent)
{
	Q_UNUSED(parent);
	Q_UNUSED(count);
	Q_UNUSED(row);
	return 0;
}

bool GridViewModel::insertColumns(int column, int count, QModelIndex const& parent)
{
	Q_UNUSED(parent);
	Q_UNUSED(count);
	Q_UNUSED(column);
	return 0;
}

bool GridViewModel::removeColumns(int column, int count, QModelIndex const& parent)
{
	Q_UNUSED(parent);
	Q_UNUSED(count);
	Q_UNUSED(column);
	return 0;
}

void GridViewModel::setModelDataAccessor(std::unique_ptr<IModelDataAccessor> accessor)
{
	if (m_accessor)
	{
		VERIFY(QObject::disconnect(m_accessor->qobject(), SIGNAL(rowAdded(int)), this, SLOT(onRowAdded(int))));
		VERIFY(QObject::disconnect(m_accessor->qobject(), SIGNAL(itemChanged(int, int)), this, SLOT(onItemChanged(int, int))));
		//Q_SIGNAL virtual void itemChanged(int row, int column) override;
	}

	std::unique_ptr<IModelDataAccessor> oldAccessor = std::move(m_accessor);
	m_accessor = std::move(accessor);

	VERIFY(QObject::connect(m_accessor->qobject(), SIGNAL(rowAdded(int)), this, SLOT(onRowAdded(int))));
	VERIFY(QObject::connect(m_accessor->qobject(), SIGNAL(itemChanged(int, int)), this, SLOT(onItemChanged(int, int))));
	emit modelDataAccessorChanged(m_accessor.get(), oldAccessor.get());
}

IModelDataAccessor* GridViewModel::modelDataAcessor() const
{
	return m_accessor.get();
}

IGridViewResizeStrategy* GridViewModel::resizeStrategy() const
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