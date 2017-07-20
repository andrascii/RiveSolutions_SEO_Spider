#include "grid_view_model.h"

namespace QuickieWebBot
{

GridViewModel::GridViewModel(QObject* parent)
	: QAbstractTableModel(parent)
{
}

int GridViewModel::rowCount(QModelIndex const& parent) const
{
	Q_UNUSED(parent);
	return m_accessor->rowCount();
}

int GridViewModel::columnCount(QModelIndex const& parent) const
{
	Q_UNUSED(parent);
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

void GridViewModel::setDataAccessor(std::unique_ptr<IModelDataAccessor> accessor)
{
	if (m_accessor)
	{
		VERIFY(QObject::disconnect(m_accessor->qobject(), SIGNAL(rowAdded(int)), this, SLOT(onRowAdded(int))));
	}

	m_accessor = std::move(accessor);

	VERIFY(QObject::connect(m_accessor->qobject(), SIGNAL(rowAdded(int)), this, SLOT(onRowAdded(int))));
	emit modelAccessorChanged(m_accessor.get());
}

IModelDataAccessor * GridViewModel::dataAcessor() const
{
	return m_accessor.get();
}

void GridViewModel::onRowAdded(int row)
{
	beginInsertRows(QModelIndex(), row, row);
	endInsertRows();
}

}