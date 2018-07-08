#include "abstract_table_model.h"

namespace SeoSpider
{

QModelIndexList AbstractTableModel::makeModelIndexesForRow(int row) const noexcept
{
	if (!rowCount())
	{
		return QModelIndexList();
	}

	QModelIndexList modelIndexes;

	for (int i = 0; i < columnCount(); ++i)
	{
		if (m_proxyModel)
		{
			modelIndexes.append(m_proxyModel->index(row, i));
		}
		else
		{
			modelIndexes.append(createIndex(row, i));
		}
	}

	return modelIndexes;
}

void AbstractTableModel::setProxyModel(QSortFilterProxyModel* proxy) noexcept
{
	m_proxyModel = proxy;
}

QSortFilterProxyModel* AbstractTableModel::proxyModel() const noexcept
{
	return m_proxyModel;
}
}
