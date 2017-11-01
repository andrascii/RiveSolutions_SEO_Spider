#include "summary_model.h"
#include "viewport_percent_resize_policy.h"

namespace SeoSpider
{

SummaryModel::SummaryModel(QObject* parent)
	: AbstractTableModel(parent)
	, m_resizePolicy(std::make_shared<ViewportPercentResizePolicy>(std::vector<int>{ 85, 15 }))
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
	}

	m_dataAccessor = accessor;

	VERIFY(connect(dataAccessor()->qobject(), SIGNAL(dataChanged(int, int, Qt::ItemDataRole)),
		this, SLOT(formActualUpdateDataSignal(int, int, Qt::ItemDataRole))));

	endResetModel();
}

ISummaryDataAccessor* SummaryModel::dataAccessor() const noexcept
{
	return m_dataAccessor;
}

StorageAdaptorType SummaryModel::storageAdaptorType(const QModelIndex& index) const noexcept
{
	if (!dataAccessor())
	{
		return StorageAdaptorType::StorageAdaptorTypeNone;
	}

	return dataAccessor()->itemCategory(index);
}

void SummaryModel::formActualUpdateDataSignal(int row, int column, Qt::ItemDataRole role)
{
	Q_EMIT dataChanged(createIndex(row, column), createIndex(row, column), QVector<int>() << role);
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
	}

	return QVariant();
}

QVariant SummaryModel::headerData(int section, Qt::Orientation orientation, int role) const
{
	return QVariant();
}

int SummaryModel::columnCount(const QModelIndex& parent) const
{
	if (!dataAccessor())
	{
		return 0;
	}

	return dataAccessor()->columnCount();
}

int SummaryModel::rowCount(const QModelIndex& parent) const
{
	if (!dataAccessor())
	{
		return 0;
	}

	return dataAccessor()->rowCount();
}

}