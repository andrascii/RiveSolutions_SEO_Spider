#include "table_proxy_model.h"
#include "abstract_table_model.h"
#include "row_resource_type.h"

namespace SeoSpider
{

TableSortFilterProxyModel::TableSortFilterProxyModel(QObject* parent)
	: QSortFilterProxyModel(parent)
	, m_acceptedResources(ResourceAny)
{
}

bool TableSortFilterProxyModel::filterAcceptsRow(int sourceRow, const QModelIndex& sourceParent) const
{
	const bool result = QSortFilterProxyModel::filterAcceptsRow(sourceRow, sourceParent);
	if (result)
	{
		const QModelIndex index = sourceModel()->index(sourceRow, 1);
		const int resourceType = sourceModel()->data(index, AbstractTableModel::resourceTypeRole).toInt();
		if (!(resourceType & m_acceptedResources))
		{
			return false;
		}
	}

	return result;
}
}
