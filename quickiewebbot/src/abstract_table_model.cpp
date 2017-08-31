#include "abstract_table_model.h"

namespace QuickieWebBot
{

QModelIndexList AbstractTableModel::modelIndexesForRow(int row) const noexcept
{
	DEBUG_ASSERT(row < rowCount());

	QModelIndexList modelIndexes;

	for (int i = 0; i < columnCount(); ++i)
	{
		modelIndexes.append(createIndex(row, i));
	}

	return modelIndexes;
}

}