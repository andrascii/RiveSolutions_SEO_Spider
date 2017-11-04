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
		modelIndexes.append(createIndex(row, i));
	}

	return modelIndexes;
}

}