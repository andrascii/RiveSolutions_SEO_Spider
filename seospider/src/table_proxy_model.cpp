#include "table_proxy_model.h"
#include "abstract_table_model.h"
#include "row_resource_type.h"

namespace SeoSpider
{

TableProxyModel::TableProxyModel(QObject* parent)
	: QSortFilterProxyModel(parent)
	, m_acceptedResources(ResourceAny)
{
}

void TableProxyModel::setAcceptedResources(int resources)
{
	m_acceptedResources = resources;
	setFilterKeyColumn(filterKeyColumn());
}

int TableProxyModel::acceptedResources() const
{
	return m_acceptedResources;
}

bool TableProxyModel::filterAcceptsRow(int sourceRow, const QModelIndex& sourceParent) const
{
	const QModelIndex index = sourceModel()->index(sourceRow, 1);
	const int resourceType = sourceModel()->data(index, AbstractTableModel::resourceTypeRole).toInt();
	if (!(resourceType & m_acceptedResources))
	{
		return false;
	}
	return QSortFilterProxyModel::filterAcceptsRow(sourceRow, sourceParent);
}
}
