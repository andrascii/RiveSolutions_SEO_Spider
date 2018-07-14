#include "summary_model.h"
#include "viewport_percent_resize_policy.h"

namespace SeoSpider
{

SummaryModel::SummaryModel(QObject* parent)
	: AbstractTableModel(parent)
	, m_resizePolicy(std::make_shared<ViewportPercentResizePolicy>(QVector<int>{ 85, 15 }))
	, m_dataAccessor(nullptr)
{
}

Qt::ItemFlags SummaryModel::flags(const QModelIndex& index) const
{
	return dataAccessor() ? dataAccessor()->flags(index) : Qt::NoItemFlags;
}

IResizePolicy* SummaryModel::resizePolicy() const noexcept
{
	return m_resizePolicy.get();
}

void SummaryModel::setDataAccessor(ISummaryDataAccessor* accessor) noexcept
{
	beginResetModel();

	if (dataAccessor())
	{
		disconnect(dataAccessor()->qobject(), SIGNAL(dataChanged(int, int, Qt::ItemDataRole)),
			this, SLOT(formActualUpdateDataSignal(int, int, Qt::ItemDataRole)));

		disconnect(dataAccessor()->qobject(), SIGNAL(beginClearData()), this, SLOT(onAboutBeginClearingData()));
		disconnect(dataAccessor()->qobject(), SIGNAL(endClearData()), this, SLOT(onAboutEndClearingData()));
	}

	m_dataAccessor = accessor;

	VERIFY(connect(dataAccessor()->qobject(), SIGNAL(dataChanged(int, int, Qt::ItemDataRole)),
		this, SLOT(formActualUpdateDataSignal(int, int, Qt::ItemDataRole))));

	VERIFY(connect(dataAccessor()->qobject(), SIGNAL(beginClearData()), this, SLOT(onAboutBeginClearingData())));
	VERIFY(connect(dataAccessor()->qobject(), SIGNAL(endClearData()), this, SLOT(onAboutEndClearingData())));

	endResetModel();

	emit internalDataChanged();
}

ISummaryDataAccessor* SummaryModel::dataAccessor() const noexcept
{
	return m_dataAccessor;
}

StorageAdapterType SummaryModel::storageAdapterType(const QModelIndex& index) const noexcept
{
	if (!dataAccessor())
	{
		return StorageAdapterType::StorageAdapterTypeNone;
	}

	return dataAccessor()->itemCategory(index);
}

void SummaryModel::formActualUpdateDataSignal(int row, int column, Qt::ItemDataRole role)
{
	Q_EMIT dataChanged(createIndex(row, column), createIndex(row, column), QVector<int>() << role);
}

void SummaryModel::onAboutBeginClearingData()
{
	beginResetModel();
}

void SummaryModel::onAboutEndClearingData()
{
	endResetModel();
}

QSize SummaryModel::span(const QModelIndex& index) const
{
	if (!dataAccessor())
	{
		return QAbstractTableModel::span(index);
	}

	return dataAccessor()->span(index);
}

QVariant SummaryModel::data(const QModelIndex& index, int role) const
{
	if (!dataAccessor())
	{
		return QVariant();
	}

	switch (role)
	{
		case Qt::AccessibleTextRole:
		case Qt::AccessibleDescriptionRole:
		case Qt::DisplayRole:
		{
			return dataAccessor()->item(index);
		}

		case Qt::DecorationRole:
		{
			return dataAccessor()->item(index);
		}

		case AbstractTableModel::underlyingIndexRole:
		{
			return index;
		}
	}

	return QVariant();
}

QVariant SummaryModel::headerData(int section, Qt::Orientation orientation, int role) const
{
	Q_UNUSED(section);
	Q_UNUSED(orientation);
	Q_UNUSED(role);

	return QVariant();
}

int SummaryModel::columnCount(const QModelIndex& parent) const
{
	Q_UNUSED(parent);

	if (!dataAccessor())
	{
		return 0;
	}

	return dataAccessor()->columnCount();
}

int SummaryModel::rowCount(const QModelIndex& parent) const
{
	Q_UNUSED(parent);

	if (!dataAccessor())
	{
		return 0;
	}

	return dataAccessor()->rowCount();
}

}